#include "hip_graph_capture/analyzer.h"
#include "hip_graph_capture/graph.h"
#include <algorithm>
#include <numeric>

namespace hip {

struct AnalysisReport {
    size_t total_kernels;
    size_t total_memcpy_ops;
    size_t unique_streams;
    double total_compute_time_us;
    double total_memcpy_time_us;
    double critical_path_time_us;
    uint32_t max_parallelism;
    size_t total_bytes_transferred;
    double compute_throughput_tflops;
    double memory_throughput_gbs;
    std::vector<std::string> bottlenecks;
    std::vector<std::string> recommendations;
};

AnalysisReport analyze_graph(const DependencyGraph& graph) {
    AnalysisReport report = {};
    report.total_kernels = 0;
    report.total_memcpy_ops = 0;
    report.max_parallelism = graph.max_parallelism();

    std::unordered_set<hipStream_t> streams;

    for (size_t i = 0; i < graph.node_count(); i++) {
        const auto& node = graph.node(i);
        if (node.kernel) {
            report.total_kernels++;
            report.total_compute_time_us += (node.kernel->end_ns - node.kernel->start_ns) / 1000.0;
            streams.insert(node.kernel->stream);
        }
        if (node.memcpy) {
            report.total_memcpy_ops++;
        }
    }

    report.unique_streams = streams.size();
    report.total_bytes_transferred = graph.total_memory_transferred();

    // Critical path analysis
    auto cp = graph.critical_path();
    report.critical_path_time_us = 0.0;
    for (uint32_t id : cp) {
        const auto& node = graph.node(id);
        if (node.kernel) {
            report.critical_path_time_us += (node.kernel->end_ns - node.kernel->start_ns) / 1000.0;
        }
    }

    // Generate recommendations
    if (report.max_parallelism < 4 && report.unique_streams == 1) {
        report.recommendations.push_back(
            "Low parallelism detected. Consider using multiple HIP streams to overlap kernels.");
    }

    double compute_ratio = report.total_compute_time_us / (report.total_compute_time_us + report.total_memcpy_time_us + 1e-9);
    if (compute_ratio < 0.5) {
        report.bottlenecks.push_back("Memory operations dominate execution time");
        report.recommendations.push_back(
            "Consider using async memcpy with hipMemcpyAsync to overlap with compute.");
    }

    if (report.total_kernels > 1000 && report.max_parallelism < 8) {
        report.recommendations.push_back(
            "Many serial kernels detected. Consider kernel fusion to reduce launch overhead.");
    }

    return report;
}

} // namespace hip
