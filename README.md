# hip-graph-capture

HIP graph capture, serialization, and replay utilities for AMD ROCm platforms.

Capture kernel launch sequences as HIP graphs, serialize them to portable formats, replay with deterministic timing, and analyze graph structure for optimization.

## Features

- **Capture**: Intercept HIP kernel launches and build dependency graphs
- **Serialize**: Export graphs to JSON, DOT (Graphviz), or binary format
- **Replay**: Replay captured graphs with configurable warmup/iterations
- **Analyze**: Identify parallelism opportunities, critical paths, memory conflicts
- **Visualize**: Generate dependency diagrams and timeline views

## Quick Start

```cpp
#include "hip_graph_capture/capture.h"

// Start capturing
hip::GraphCapture capture;
capture.begin();

// Your HIP code runs normally
my_kernel<<<grid, block>>>(d_a, d_b, d_c, n);
reduce_kernel<<<1, 256>>>(d_c, d_out, n);

// Stop and get the graph
auto graph = capture.end();
graph.serialize("trace.json");
graph.visualize("trace.dot");
```

## CLI Tool

```bash
# Capture from a running application
hip-gcapture --app ./my_hip_app --output trace.json

# Replay a captured graph
hip-gcapture replay --trace trace.json --iterations 1000

# Analyze graph structure
hip-gcapture analyze --trace trace.json --critical-path

# Generate visualization
hip-gcapture visualize --trace trace.json --format dot > graph.dot
dot -Tpng graph.dot -o graph.png
```

## Architecture

```
hip-graph-capture/
├── include/hip_graph_capture/
│   ├── capture.h         # Main capture API
│   ├── graph.h           # Graph data structure
│   ├── serializer.h      # JSON/DOT/binary exporters
│   └── analyzer.h        # Graph analysis algorithms
├── src/
│   ├── capture.cpp       # HIP API interception via LD_PRELOAD
│   ├── graph.cpp         # DAG construction and manipulation
│   ├── serializer.cpp    # Serialization implementations
│   ├── analyzer.cpp      # Critical path, parallelism analysis
│   └── replay.cpp        # Graph replay engine
├── tools/
│   └── hip-gcapture/     # CLI tool
├── tests/
│   ├── test_capture.cpp  # Unit tests
│   └── test_analyze.cpp  # Analysis tests
├── examples/
│   ├── basic/            # Simple capture example
│   └── multi_stream/     # Multi-stream graph capture
└── docs/
    ├── api.md            # C++ API reference
    └── hip_graphs.md     # HIP graph fundamentals
```

## Related Projects

Part of the [ROCm Developer Toolkit](https://github.com/huang-poi) ecosystem:
- [rocprobe](https://github.com/huang-poi/rocprobe) — GPU profiler CLI
- [rocblas-lite](https://github.com/huang-poi/rocblas-lite) — Rust rocBLAS bindings
- [mi300x-bench](https://github.com/huang-poi/mi300x-bench) — MI300X benchmarks
- [hip-kernel-lab](https://github.com/huang-poi/hip-kernel-lab) — Kernel examples
- [rocm-devbox](https://github.com/huang-poi/rocm-devbox) — Dev environment configs

## License

MIT
