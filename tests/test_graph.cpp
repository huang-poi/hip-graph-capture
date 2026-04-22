#include <cassert>
#include <iostream>
#include "hip_graph_capture/graph.h"

void test_basic_graph() {
    hip::DependencyGraph graph;

    hip::GraphNode n1;
    n1.type = hip::GraphNode::Kernel;
    n1.kernel = hip::KernelLaunch{"kernel_a", {256,1,1}, {64,1,1}, 0, nullptr, 1000, 2000, 1024, 512};
    graph.add_node(std::move(n1));

    hip::GraphNode n2;
    n2.type = hip::GraphNode::Kernel;
    n2.kernel = hip::KernelLaunch{"kernel_b", {128,1,1}, {64,1,1}, 0, nullptr, 2500, 3500, 2048, 1024};
    graph.add_node(std::move(n2));

    graph.add_dependency(0, 1);

    assert(graph.node_count() == 2);
    assert(graph.edge_count() == 1);

    auto sorted = graph.topological_sort();
    assert(sorted.size() == 2);
    assert(sorted[0] == 0);
    assert(sorted[1] == 1);

    assert(graph.total_kernel_time_us() == 2000.0);
    assert(graph.total_memory_transferred() == 3584);

    std::cout << "test_basic_graph: PASSED\n";
}

void test_parallel_graph() {
    hip::DependencyGraph graph;

    for (int i = 0; i < 4; i++) {
        hip::GraphNode n;
        n.type = hip::GraphNode::Kernel;
        n.kernel = hip::KernelLaunch{"kernel_" + std::to_string(i), {64,1,1}, {64,1,1}, 0, nullptr, 1000, 2000, 512, 256};
        graph.add_node(std::move(n));
    }

    // All 4 kernels are independent — max parallelism should be 4
    assert(graph.max_parallelism() == 4);
    std::cout << "test_parallel_graph: PASSED\n";
}

int main() {
    test_basic_graph();
    test_parallel_graph();
    std::cout << "All tests passed!\n";
    return 0;
}
