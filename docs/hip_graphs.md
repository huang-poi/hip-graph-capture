# HIP Graph Fundamentals

## What are HIP Graphs?

HIP graphs represent a sequence of operations (kernels, memory copies, synchronization) as a directed acyclic graph (DAG). Unlike stream-based execution, graphs allow the HIP runtime to:

1. **Optimize launch overhead** — amortize per-kernel launch costs across the entire graph
2. **Expose parallelism** — automatically identify independent operations
3. **Enable scheduling** — the runtime can reorder operations for better hardware utilization

## When to Use Graphs

- **Repetitive workloads**: Same kernel sequence executed many times (e.g., inference loops)
- **Complex dependencies**: Many kernels with non-trivial dependency patterns
- **Performance-critical paths**: When stream launch overhead is measurable

## Graph Capture Modes

### Stream Capture
```cpp
hipStream_t stream;
hipStreamCreate(&stream);

hipStreamBeginCapture(stream, hipStreamCaptureModeGlobal);

// Enqueue operations — they are recorded, not executed
kernel_A<<<grid, block, 0, stream>>>(args);
kernel_B<<<grid, block, 0, stream>>>(args);

hipGraph_t graph;
hipStreamEndCapture(stream, &graph);
```

### Explicit Graph Construction
```cpp
hipGraph_t graph;
hipGraphCreate(&graph, 0);

hipGraphNode_t kernel_node;
hipKernelNodeParams params = {};
params.func = (void*)my_kernel;
params.gridDim = grid;
params.blockDim = block;
hipGraphAddKernelNode(&kernel_node, graph, nullptr, 0, &params);
```

## Performance Considerations

| Aspect | Stream | Graph |
|--------|--------|-------|
| Launch overhead | ~5-10μs per kernel | ~1-2μs amortized |
| Flexibility | Fully dynamic | Fixed structure |
| Memory | Lower | Graph storage overhead |
| Best for | Dynamic workloads | Repetitive patterns |

## hip-graph-capture Integration

```bash
# Capture and analyze your workload
hip-gcapture capture --app ./my_workload --output trace.json
hip-gcapture analyze --trace trace.json --critical-path
hip-gcapture visualize --trace trace.json --format dot
```

The analysis will identify:
- Critical path (longest dependency chain)
- Maximum parallelism level
- Memory transfer bottlenecks
- Kernel fusion opportunities
