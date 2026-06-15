# Database Query Execution Optimizer

A terminal visualiser that shows how a database query optimiser *thinks*. It
turns a simple search into an animated walk through the core data structures
from the case study, narrated in plain English by a friendly guide.

Built in C++ with [FTXUI](https://github.com/ArthurSonzogni/FTXUI).

## Two programs

| Program | What it is |
|---|---|
| **`homepage`** | The full interactive TUI (FTXUI): KNOW MORE, SIMULATE (CollegeDB / InstaDB live search), and a ROLLBACK SIMULATION. |
| **`src/skeleton.cpp`** | A tiny no-UI console version (only `<iostream>` + `<string>`) showing the same optimiser logic. |

## Case-study features → data structures

| Feature | Structure | File |
|---|---|---|
| Query Index | AVL / balanced tree | `src/Query_index.cpp` |
| Execution Log (rollback) | Stack | `src/Execution_log.cpp` |
| Worker Pipeline | FIFO Queue | `src/worker_pipeline.cpp` |
| Result Cache | Hash map | `src/result_cache.cpp` |
| Plan Sorter | Min-heap | `src/plan_sorter.cpp` |
| Join Map | Graph | `src/join_map.cpp` |
| Fastest Path | Dijkstra | `src/fastest_path.cpp` |
| Resource Balancer | Greedy allocation | `src/resource_balancer.cpp` |

`src/main.cpp` holds the box/button styling and wires the screens together;
`src/ui.cpp` holds the shared FTXUI rendering, datasets, timeline and bunny.

## Build & run

### Full TUI (needs CMake; FTXUI is fetched automatically)
```bash
cmake -S . -B build
cmake --build build -j4
./build/homepage
```

### Skeleton (no dependencies)
```bash
cd src
g++ skeleton.cpp -o skeleton
./skeleton
```
