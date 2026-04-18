#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <cstdint>
#include <hip/hip_runtime.h>

namespace hip {

struct KernelLaunch {
    std::string name;
    dim3 grid;
    dim3 block;
    size_t shared_mem;
    hipStream_t stream;
    uint64_t start_ns;
    uint64_t end_ns;
    size_t bytes_read;
    size_t bytes_written;
};

struct MemcpyOp {
    void* dst;
    const void* src;
    size_t size;
    hipMemcpyKind kind;
    hipStream_t stream;
    uint64_t start_ns;
    uint64_t end_ns;
};

struct GraphNode {
    enum Type { Kernel, Memcpy, Memset, EventRecord, EventSynchronize, Barrier };
    Type type;
    uint32_t id;
    std::vector<uint32_t> dependencies;
    std::vector<uint32_t> dependents;
    std::optional<KernelLaunch> kernel;
    std::optional<MemcpyOp> memcpy;
};

class DependencyGraph {
public:
    void add_node(GraphNode node);
    void add_dependency(uint32_t from, uint32_t to);
    std::vector<uint32_t> topological_sort() const;
    std::vector<uint32_t> critical_path() const;
    uint32_t max_parallelism() const;
    size_t total_memory_transferred() const;
    double total_kernel_time_us() const;

    void serialize_json(const std::string& path) const;
    void serialize_dot(const std::string& path) const;
    void serialize_binary(const std::string& path) const;
    static DependencyGraph deserialize_json(const std::string& path);

    size_t node_count() const { return nodes_.size(); }
    size_t edge_count() const;
    const GraphNode& node(uint32_t id) const { return nodes_.at(id); }

private:
    std::unordered_map<uint32_t, GraphNode> nodes_;
    uint32_t next_id_ = 0;
};

} // namespace hip
