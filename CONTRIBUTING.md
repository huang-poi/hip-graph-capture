# Contributing to hip-graph-capture

## Development Setup

```bash
# Requires ROCm 6.0+
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
ctest
```

## Commit Convention

```
feat(capture): add LD_PRELOAD-based kernel interception
fix(graph): correct topological sort for cyclic detection
docs(api): add C++ API reference
test(analyze): add critical path test cases
```

## Architecture

- `src/capture.cpp` — HIP API interception via LD_PRELOAD or roctracer
- `src/graph.cpp` — DAG data structure, topo sort, critical path
- `src/analyzer.cpp` — Performance analysis algorithms
- `src/replay.cpp` — Graph replay engine
- `tools/hip-gcapture/` — CLI interface
