# Database Query Execution Optimizer — Project Report

A C++ project that **visualises how a database query optimiser "thinks"** and
also ships the same logic as a tiny, dependency-free program. It covers all the
"must-have" features of the *Database Query Execution Optimizer* case study and
maps each one to a classic data structure.

---

## 1. What the project is (in one paragraph)

When you search for something in a huge database, the database doesn't just scan
every row — it **plans** the cheapest way to find the answer using several data
structures working together (an index, a cache, a cost ranker, a relationship
graph, a shortest-path finder, a job queue, an undo log). This project makes
that invisible process **visible**: you type a name, and the program walks you
through every step — cache check → index lookup → plan generation → best-plan
selection → relationship graph → fastest join route → resource allocation →
worker queue → execution stack → result — narrated in plain English. It comes in
two forms: a polished animated terminal app, and a bare-bones console version.

---

## 2. File inventory — how many files and what each is for

Everything lives in **`src/`**. (The big `build/` folder is *not* part of the
code — it only holds CMake output and the auto-downloaded FTXUI library, and is
git-ignored.)

| File | Lines | Purpose |
|---|---:|---|
| `src/main.cpp` | 506 | Entry point. Box styling, button styling, screen wiring (HOME, SIMULATE confirm, the two search pages, KNOW MORE, ROLLBACK), the animation clock, and `main()`. `#include`s the other files (unity build). |
| `src/ui.cpp` | 493 | All shared UI: colour language, helpers, the QDATA banner, the bunny narrator, the loading animation, the datasets, the 11-stage timeline, result cards, explanations, KNOW MORE page. |
| `src/Query_index.cpp` | 63 | **Query Index** feature → AVL tree index search view. |
| `src/Execution_log.cpp` | 135 | **Execution Log** feature → stack view **+ the whole ROLLBACK simulation**. |
| `src/plan_sorter.cpp` | 37 | **Plan Sorter** feature → plan cards + min-heap selection view. |
| `src/resource_balancer.cpp` | 29 | **Resource Balancer** feature → server-load allocation view. |
| `src/fastest_path.cpp` | 28 | **Fastest Path** feature → Dijkstra route view. |
| `src/join_map.cpp` | 20 | **Join Map** feature → relationship graph view. |
| `src/result_cache.cpp` | 19 | **Result Cache** feature → cache scan / HIT-MISS view. |
| `src/worker_pipeline.cpp` | 19 | **Worker Pipeline** feature → FIFO queue conveyor view. |
| `src/skeleton.cpp` | 104 | **The actual algorithms, no UI** — only `<iostream>` + `<string>`. |
| `CMakeLists.txt` | — | Build config; fetches FTXUI automatically. |
| `README.md` | — | Quick start. |
| `project-overview.excalidraw` | — | The visual one-page overview board. |

**Counts:** 11 `.cpp` files (≈ **1,453 lines** of C++ total).

---

## 3. Which folder/file actually has the "working code" (excluding the UI)

- **All source is in `src/`.**
- The TUI app (`main.cpp`, `ui.cpp`, and the 8 feature files = **≈ 1,349 lines**)
  is a **visualiser** — those files build animated screens with FTXUI. The values
  they show (tree numbers, plan costs, the route) are scripted for teaching.
- The **genuinely UI-free, real working code is `src/skeleton.cpp` (104 lines)**.
  It actually *runs* the algorithms — binary-search index, a real Dijkstra, a
  cheapest-plan pick, a stack rollback, a FIFO queue — and prints text. No FTXUI.

So: **if you want the bare algorithmic implementation, it is `src/skeleton.cpp`.**
The 8 feature files express the *same ideas* but as animations.

**Line totals**
- 8 feature files together: **≈ 350 lines**
- `skeleton.cpp`: **104 lines**
- UI engine (`main.cpp` + `ui.cpp`): **999 lines**

---

## 4. Why it is structured this way

1. **One feature → one file.** The case study lists 8 must-have features. Each
   gets its own file (`Query_index.cpp`, `Execution_log.cpp`, …) so the mapping
   *feature ⇄ data structure ⇄ code* is obvious and easy to grade/defend.
2. **UI separated from the per-feature logic.** `ui.cpp` holds everything shared
   (colours, banner, bunny, timeline, datasets); `main.cpp` holds styling and the
   screen flow. The feature files only describe *their* stage. This keeps each
   file small and focused.
3. **Unity build (`main.cpp` `#include`s the rest).** Simple to compile (CMake
   builds one target) and lets the files stay header-free while still sharing
   helpers. Trade-off: editing a feature file means rebuilding `main.cpp`.
4. **A separate `skeleton.cpp`.** A no-dependency proof of the logic that anyone
   can run with a single `g++` command — useful for a viva or a quick demo where
   installing FTXUI isn't worth it.

---

## 5. How it works (architecture)

**The animation engine.** A background "ticker" thread repaints the screen ~11×
a second. A wall-clock timestamp (`run_start`) is turned into a `(stage,
progress)` pair by a fixed **timeline** of 11 stages. Each render call asks "what
stage am I in and how far through it?" and draws that stage. Smooth motion comes
free — progress bars fill, cards reveal, the bunny waves and blinks.

**The screens (homepage buttons):**
- **KNOW MORE** → an info page (what the project is + the 5 answers).
- **SIMULATE** → choose **CollegeDB** (search a student) or **InstaDB** (search a
  profile), type a name, and the 11-stage pipeline animates live. Layout: left =
  search box + results, middle = live data structure + bunny narrator, right =
  the DS list that lights up as each structure is used.
- **ROLLBACK SIMULATION** → demonstrates failure recovery: operations are pushed
  onto a stack, an **OOPS! error** flashes across the screen, then the stack
  **unwinds** (pops/undoes in reverse) back to a stable state — then asks "watch
  again? YES/NO".

**Two themed datasets** prove the same engine works for any domain: a college
ERP (students with roll no. + cohort) and a social network (profiles with
followers).

---

## 6. Each feature — how it works + real-world MySQL / PostgreSQL link

| # | Feature | Data structure | How it works here | In MySQL / PostgreSQL |
|---|---|---|---|---|
| 1 | **Query Index** | AVL / balanced tree | Instead of scanning every record, it walks a balanced tree to the match in O(log n). In `skeleton.cpp` this is a real **binary search** on a sorted array. | Both use **B+-tree indexes** (InnoDB primary/secondary indexes; PostgreSQL B-tree). Same goal: O(log n) lookups instead of full-table scans. |
| 2 | **Execution Log** | Stack (LIFO) | Each operation is *pushed* as it runs; on error the stack is *popped* in reverse to undo work and restore the last good state. | **Undo logs / WAL** + `ROLLBACK` and `SAVEPOINT`. InnoDB undo logs revert a failed transaction; PostgreSQL uses WAL + MVCC. |
| 3 | **Worker Pipeline** | FIFO Queue | Background jobs (rebuild index, update stats, cleanup) wait in line and are processed in arrival order so they never block the user query. | Background workers: PostgreSQL **autovacuum / background writer / checkpointer**, InnoDB **purge & I/O threads**, replication apply queues. |
| 4 | **Result Cache** | Hash map | First thing checked: was this exact result computed recently? HIT → return instantly; MISS → do the work and store it. O(1) average lookup. | MySQL's old **query cache**, prepared-statement & plan caches, PostgreSQL **shared_buffers** page cache; external **Redis/Memcached**. |
| 5 | **Plan Sorter** | Min-heap / priority queue | Several candidate plans are generated with estimated costs; a min-heap surfaces the **cheapest** one instantly (e.g., 120 vs 40 vs 10 → pick 10). | **Cost-based optimizer.** Both DBs estimate a cost for each plan and pick the lowest — `EXPLAIN` shows those costs. |
| 6 | **Join Map** | Graph | Tables/entities are nodes; relationships (foreign keys) are edges. Drawn as a small graph (e.g., Student–Courses–Results). | The planner's internal model of tables and **foreign-key relationships** used to decide what can be joined to what. |
| 7 | **Fastest Path** | Dijkstra (shortest path) | Edges get weights (estimated cost); Dijkstra finds the cheapest route through the join graph (e.g., Student → Courses → Results, cost 3). `skeleton.cpp` runs a real Dijkstra on a 3×3 matrix. | **Join-order optimisation** — choosing the cheapest sequence of joins. PostgreSQL searches join orders (dynamic programming, GEQO for many tables); same "lowest-cost path" idea. |
| 8 | **Resource Balancer** | Greedy allocation | Worker/server loads are scanned and the query is routed to the **least-busy** one (Server A 80% / B 40% / C 10% → C). | Dynamic resource allocation: **parallel worker assignment**, `work_mem`, connection pooling, read-replica **load balancing**. |

---

## 7. The five case-study questions — answered

> **1. How was database information indexed for fast retrieval?**
> With a **balanced search tree (AVL / B-tree idea)** — the *Query Index*
> (`Query_index.cpp`; real binary search in `skeleton.cpp`). Lookups take
> **O(log n)** instead of an O(n) full scan. This is exactly what MySQL/PostgreSQL
> B+-tree indexes do.

> **2. How was query execution history maintained to support rollback?**
> With a **Stack (LIFO)** — the *Execution Log* (`Execution_log.cpp`). Every
> operation is pushed as it executes; if a step fails, the stack is popped in
> reverse, undoing each operation back to the last stable state. Mirrors database
> **undo logs / WAL** and the `ROLLBACK` command.

> **3. How were query requests processed in the order they arrived?**
> With a **FIFO Queue** — the *Worker Pipeline* (`worker_pipeline.cpp`). Jobs are
> served first-in-first-out, so requests/background tasks run in arrival order
> without blocking, like a DB's background-worker queues.

> **4. How were relationships between database tables represented?**
> With a **Graph** — the *Join Map* (`join_map.cpp`). Tables are nodes and
> foreign-key relationships are edges, exactly how a planner models which tables
> can join to which.

> **5. How was the most efficient query execution path determined?**
> With **Dijkstra's shortest-path algorithm** over the weighted join graph — the
> *Fastest Path* (`fastest_path.cpp`; real Dijkstra in `skeleton.cpp`). Edge
> weights = estimated costs, and the cheapest route wins — the same principle as a
> **cost-based optimiser's join-order selection**.

---

## 8. How to build & run

**Full terminal app (needs CMake; FTXUI is fetched automatically):**
```bash
cmake -S . -B build
cmake --build build -j4
./build/homepage
```

**The bare-logic skeleton (no dependencies):**
```bash
cd src
g++ skeleton.cpp -o skeleton
./skeleton          # type a name (e.g. raheesh) or just press Enter
```

---

## 9. Things worth knowing / honest notes

- The TUI is a **teaching visualiser**: the numbers it animates (tree values,
  costs, the route) are scripted to tell a clear story. The literal, runnable
  algorithms live in **`skeleton.cpp`**.
- The two databases (**CollegeDB**, **InstaDB**) share one engine — only the data
  and labels differ, showing the optimiser idea is domain-independent.
- Search works by **first name OR surname** in CollegeDB; an unknown name returns
  "NOT FOUND".
- Repo: `github.com/paarshviv12/DATABASE-QUERY-EXECUTION-OPTIMIZER-`.
- A one-page visual summary is in `project-overview.excalidraw`.

---

*Summary:* 11 C++ files (~1,453 lines) in `src/`; one file per case-study
feature, a shared UI engine, and a 104-line no-UI `skeleton.cpp` that runs the
real algorithms. Each feature maps to a textbook data structure and to a real
mechanism inside MySQL/PostgreSQL.
