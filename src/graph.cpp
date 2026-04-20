#include "hip_graph_capture/graph.h"
#include <queue>
#include <algorithm>
#include <fstream>
#include <stack>
#include <limits>

namespace hip {

void DependencyGraph::add_node(GraphNode node) {
    node.id = next_id_++;
    nodes_[node.id] = std::move(node);
}

void DependencyGraph::add_dependency(uint32_t from, uint32_t to) {
    if (nodes_.count(from) && nodes_.count(to)) {
        nodes_[from].dependents.push_back(to);
        nodes_[to].dependencies.push_back(from);
    }
}

std::vector<uint32_t> DependencyGraph::topological_sort() const {
    std::unordered_map<uint32_t, int> in_degree;
    for (auto& [id, node] : nodes_) {
        in_degree[id] = node.dependencies.size();
    }

    std::queue<uint32_t> q;
    for (auto& [id, degree] : in_degree) {
        if (degree == 0) q.push(id);
    }

    std::vector<uint32_t> result;
    while (!q.empty()) {
        uint32_t current = q.front();
        q.pop();
        result.push_back(current);

        for (uint32_t dep : nodes_.at(current).dependents) {
            in_degree[dep]--;
            if (in_degree[dep] == 0) q.push(dep);
        }
    }
    return result;
}

std::vector<uint32_t> DependencyGraph::critical_path() const {
    auto sorted = topological_sort();
    std::unordered_map<uint32_t, double> dist;
    std::unordered_map<uint32_t, uint32_t> parent;

    for (uint32_t id : sorted) {
        dist[id] = 0.0;
    }

    for (uint32_t id : sorted) {
        const auto& node = nodes_.at(id);
        double duration = 0.0;
        if (node.kernel) {
            duration = (node.kernel->end_ns - node.kernel->start_ns) / 1000.0;
        }

        for (uint32_t dep : node.dependents) {
            double new_dist = dist[id] + duration;
            if (new_dist > dist[dep]) {
                dist[dep] = new_dist;
                parent[dep] = id;
            }
        }
    }

    // Find the node with maximum distance
    uint32_t end_node = 0;
    double max_dist = 0;
    for (auto& [id, d] : dist) {
        if (d > max_dist) { max_dist = d; end_node = id; }
    }

    // Trace back
    std::vector<uint32_t> path;
    uint32_t current = end_node;
    while (parent.count(current)) {
        path.push_back(current);
        current = parent[current];
    }
    path.push_back(current);
    std::reverse(path.begin(), path.end());
    return path;
}

uint32_t DependencyGraph::max_parallelism() const {
    auto sorted = topological_sort();
    // BFS level count
    std::unordered_map<uint32_t, int> level;
    uint32_t max_par = 0;

    for (uint32_t id : sorted) {
        int max_dep_level = 0;
        for (uint32_t dep : nodes_.at(id).dependencies) {
            max_dep_level = std::max(max_dep_level, level[dep] + 1);
        }
        level[id] = max_dep_level;
    }

    // Count nodes at each level
    std::unordered_map<int, int> level_count;
    for (auto& [id, l] : level) {
        level_count[l]++;
        max_par = std::max(max_par, (uint32_t)level_count[l]);
    }

    return max_par;
}

size_t DependencyGraph::total_memory_transferred() const {
    size_t total = 0;
    for (auto& [id, node] : nodes_) {
        if (node.kernel) {
            total += node.kernel->bytes_read + node.kernel->bytes_written;
        }
        if (node.memcpy) {
            total += node.memcpy->size;
        }
    }
    return total;
}

double DependencyGraph::total_kernel_time_us() const {
    double total = 0.0;
    for (auto& [id, node] : nodes_) {
        if (node.kernel) {
            total += (node.kernel->end_ns - node.kernel->start_ns) / 1000.0;
        }
    }
    return total;
}

size_t DependencyGraph::edge_count() const {
    size_t count = 0;
    for (auto& [id, node] : nodes_) {
        count += node.dependencies.size();
    }
    return count;
}

void DependencyGraph::serialize_json(const std::string& path) const {
    // Simplified JSON output
    std::ofstream out(path);
    out << "{\n  \"nodes\": [\n";
    bool first = true;
    for (auto& [id, node] : nodes_) {
        if (!first) out << ",\n";
        first = false;
        out << "    {\"id\": " << id << ", \"type\": " << node.type;
        if (node.kernel) {
            out << ", \"name\": \"" << node.kernel->name << "\"";
            out << ", \"duration_us\": " << (node.kernel->end_ns - node.kernel->start_ns) / 1000.0;
        }
        out << ", \"deps\": [";
        for (size_t i = 0; i < node.dependencies.size(); i++) {
            if (i > 0) out << ", ";
            out << node.dependencies[i];
        }
        out << "]}";
    }
    out << "\n  ]\n}\n";
}

void DependencyGraph::serialize_dot(const std::string& path) const {
    std::ofstream out(path);
    out << "digraph HIPGraph {\n";
    out << "  rankdir=LR;\n  node [shape=box];\n";
    for (auto& [id, node] : nodes_) {
        std::string label = "Node " + std::to_string(id);
        if (node.kernel) label = node.kernel->name;
        out << "  " << id << " [label=\"" << label << "\"];\n";
        for (uint32_t dep : node.dependencies) {
            out << "  " << dep << " -> " << id << ";\n";
        }
    }
    out << "}\n";
}

DependencyGraph DependencyGraph::deserialize_json(const std::string& path) {
    // Placeholder — would use nlohmann/json in production
    return DependencyGraph();
}

} // namespace hip
