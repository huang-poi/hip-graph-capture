#pragma once
#include "graph.h"
#include <atomic>
#include <thread>
#include <mutex>

namespace hip {

class GraphCapture {
public:
    GraphCapture();
    ~GraphCapture();

    // Start capturing kernel launches
    void begin();

    // Stop capturing and return the dependency graph
    DependencyGraph end();

    // Check if currently capturing
    bool is_capturing() const { return capturing_.load(); }

    // Set capture options
    void set_max_kernels(size_t max) { max_kernels_ = max; }
    void set_capture_memcpy(bool enable) { capture_memcpy_ = enable; }
    void set_capture_events(bool enable) { capture_events_ = enable; }

private:
    std::atomic<bool> capturing_{false};
    size_t max_kernels_ = 100000;
    bool capture_memcpy_ = true;
    bool capture_events_ = false;
    std::mutex mutex_;
    DependencyGraph graph_;
    std::vector<GraphNode> pending_nodes_;
    std::thread interceptor_;

    void intercept_loop();
    static void hip_callback(void* user_data, const void* callback_data, size_t size);
};

// RAII capture helper
class ScopedCapture {
public:
    ScopedCapture() { capture_.begin(); }
    ~ScopedCapture() { graph_ = capture_.end(); }
    const DependencyGraph& graph() const { return graph_; }

private:
    GraphCapture capture_;
    DependencyGraph graph_;
};

} // namespace hip
