# Forge - Mini Dependency-Aware Build System

Forge is a developer tool in C++17 that automates building a C++ project by:
1. Parsing a configuration file (`forge.conf`).
2. Discovering header/source dependencies dynamically.
3. Modeling relationships in a dependency Directed Acyclic Graph (DAG) with cycle detection.
4. Planning incremental builds based on file timestamps and dependency status.
5. Executing compilation tasks concurrently in a custom thread pool.
6. Launching compiler/linker commands using POSIX process APIs (`fork`, `exec`, `waitpid`).
7. Gracefully handling and propagating errors.

This project will be located in `/Users/shanitsinghal/Desktop/forge/`.

## User Review Required

Please review the proposed structure, stack, and features:
- **Stack**: C++17, CMake, POSIX process APIs (`fork`/`exec`), `std::thread`, `std::mutex`, `std::condition_variable`.
- **Location**: All files will be placed inside `/Users/shanitsinghal/Desktop/forge/`.

> [!IMPORTANT]
> **Supported Languages**:
> - **Primary**: C and C++ (e.g., `.cpp`, `.cc`, `.cxx`, `.c`, `.h`, `.hpp`, `.hxx`).
> - **Extension**: Any language with a compile-to-object and link workflow (e.g., CUDA `.cu`, Objective-C `.m`/`.mm`) by configuring the `compiler` key (e.g., `nvcc`, `clang`).
> - **Dependency Detection**: The analyzer detects `#include "..."` patterns. Languages with different import mechanisms (e.g. Go, Rust, Java) are out of scope.

> [!TIP]
> **Enhanced Failure Handling**:
> 1. **Partial Output Cleanup**: If a compilation fails, the scheduler deletes any partially written `.o` file to prevent stale/corrupt binary files from being used in future runs.
> 2. **Output Buffering**: We will capture stdout/stderr for each child process and print them as a clean consolidated block only when the task completes. This avoids interleaving console output when compiling in parallel.
> 3. **Fail-Fast Mode (Default)**: Immediately stop dispatching new compile tasks if any task fails.
> 4. **Keep-Going Mode (`-k` / `--keep-going`)**: Add a `-k` flag to the CLI. If a compilation fails, Forge will continue compiling other independent tasks that do not depend on the failed task, reporting all failures at the end.

## Open Questions

None.

---

## Proposed Changes

We will create the new project components under `/Users/shanitsinghal/Desktop/forge/`.

### 1. Build and Config System

#### [NEW] [CMakeLists.txt](file:///Users/shanitsinghal/Desktop/forge/CMakeLists.txt)
Main CMake configuration for the forge executable and its tests.

#### [NEW] [forge.conf](file:///Users/shanitsinghal/Desktop/forge/forge.conf)
Example configuration file specifying the compiler, flags, sources, includes, and target binary.

#### [NEW] [config.h](file:///Users/shanitsinghal/Desktop/forge/src/config/config.h)
Structure representing parsed configuration:
```cpp
struct BuildConfig {
    std::string compiler;
    std::string flags;
    std::vector<std::string> sources;
    std::vector<std::string> includes;
    std::string output;
};
```

#### [NEW] [config_parser.cpp](file:///Users/shanitsinghal/Desktop/forge/src/config/config_parser.cpp)
Parses the key-value config file (e.g. `compiler = g++`, `sources = src/main.cpp src/math.cpp`).

---

### 2. Command Line Interface

#### [NEW] [command_parser.h](file:///Users/shanitsinghal/Desktop/forge/src/cli/command_parser.h)
Header declaration for CLI argument parsing.

#### [NEW] [command_parser.cpp](file:///Users/shanitsinghal/Desktop/forge/src/cli/command_parser.cpp)
Parses command-line options: `build`, `clean`, `graph`, `-j <num_threads>`, and `-k` (keep-going).

#### [NEW] [main.cpp](file:///Users/shanitsinghal/Desktop/forge/src/cli/main.cpp)
Application entry point orchestrating config loading, dependency analysis, planning, scheduling, and logging.

---

### 3. Dependency Analyzer & Graph

#### [NEW] [dependency_graph.h](file:///Users/shanitsinghal/Desktop/forge/src/dependency/dependency_graph.h)
Header for `DependencyGraph` class, storing node relationships using an adjacency list.

#### [NEW] [dependency_graph.cpp](file:///Users/shanitsinghal/Desktop/forge/src/dependency/dependency_graph.cpp)
Implements graph insertion, traversal, topological sorting (Kahn's algorithm), and dependency query interfaces.

#### [NEW] [cycle_detector.h](file:///Users/shanitsinghal/Desktop/forge/src/dependency/cycle_detector.h)
Header for cycle detection helper.

#### [NEW] [cycle_detector.cpp](file:///Users/shanitsinghal/Desktop/forge/src/dependency/cycle_detector.cpp)
Implements 3-color DFS to detect dependency cycles and print clear cycle paths.

#### [NEW] [dependency_analyzer.h](file:///Users/shanitsinghal/Desktop/forge/src/dependency/dependency_analyzer.h)
Header for `DependencyAnalyzer`.

#### [NEW] [dependency_analyzer.cpp](file:///Users/shanitsinghal/Desktop/forge/src/dependency/dependency_analyzer.cpp)
Scans `.cpp` and `.h` source files for `#include "..."` patterns and resolves paths relative to the source directory and include directories.

---

### 4. Build Planner

#### [NEW] [build_task.h](file:///Users/shanitsinghal/Desktop/forge/src/planner/build_task.h)
Defines the `BuildTask` struct:
```cpp
enum class TaskState { PENDING, READY, RUNNING, SUCCESS, FAILED, BLOCKED };

struct BuildTask {
    std::string name;
    std::vector<std::string> command;
    std::string output;
    std::vector<std::string> dependencies;
    TaskState state = TaskState::PENDING;
};
```

#### [NEW] [build_planner.h](file:///Users/shanitsinghal/Desktop/forge/src/planner/build_planner.h)
Header for the `BuildPlanner`.

#### [NEW] [build_planner.cpp](file:///Users/shanitsinghal/Desktop/forge/src/planner/build_planner.cpp)
Constructs tasks for compiling source files to object files, and a final task for linking.

#### [NEW] [dirty_checker.h](file:///Users/shanitsinghal/Desktop/forge/src/planner/dirty_checker.h)
Header for checking file modification times.

#### [NEW] [dirty_checker.cpp](file:///Users/shanitsinghal/Desktop/forge/src/planner/dirty_checker.cpp)
Performs timestamp checks and recursive dirty propagation with memoization:
- If output does not exist -> rebuild.
- If source is newer than output -> rebuild.
- If any transitive header dependency is newer than output -> rebuild.

---

### 5. Process Executor

#### [NEW] [process_executor.h](file:///Users/shanitsinghal/Desktop/forge/src/executor/process_executor.h)
Header for `ProcessExecutor`.

#### [NEW] [process_executor.cpp](file:///Users/shanitsinghal/Desktop/forge/src/executor/process_executor.cpp)
Forks and execs the compiler command, waits for completion, reads exit code, and reports success/failure.

#### [NEW] [linker.h](file:///Users/shanitsinghal/Desktop/forge/src/executor/linker.h)
Header for linking tasks.

#### [NEW] [linker.cpp](file:///Users/shanitsinghal/Desktop/forge/src/executor/linker.cpp)
Constructs and runs the command to link object files into the final executable.

---

### 6. Scheduler & Thread Pool

#### [NEW] [thread_pool.h](file:///Users/shanitsinghal/Desktop/forge/src/scheduler/thread_pool.h)
Header for `ThreadPool`.

#### [NEW] [thread_pool.cpp](file:///Users/shanitsinghal/Desktop/forge/src/scheduler/thread_pool.cpp)
Implements a reusable queue-based worker pool using `std::thread`, `std::mutex`, and `std::condition_variable`.

#### [NEW] [scheduler.h](file:///Users/shanitsinghal/Desktop/forge/src/scheduler/scheduler.h)
Header for dependency-aware build scheduling.

#### [NEW] [scheduler.cpp](file:///Users/shanitsinghal/Desktop/forge/src/scheduler/scheduler.cpp)
Implements scheduler logic:
1. Initialize task dependency counters.
2. Push all tasks with `remainingDependencies == 0` into the thread pool.
3. Protect shared state (counters, queues, logs) with a mutex.
4. Upon successful completion of a task, decrement counts of dependent tasks. If their count reaches 0, queue them.
5. If a task fails:
   - Mark all downstream dependent tasks as `BLOCKED`.
   - Delete the failed task's partial/corrupt `.o` output file.
   - If **Fail-Fast** (default) is enabled, do not queue any new tasks and exit early.
   - If **Keep-Going** (`-k`) is enabled, continue executing unrelated sub-trees in parallel, only aborting at the link step.

---

### 7. Logging & Utilities

#### [NEW] [logger.h](file:///Users/shanitsinghal/Desktop/forge/src/logging/logger.h)
Header for console logging.

#### [NEW] [logger.cpp](file:///Users/shanitsinghal/Desktop/forge/src/logging/logger.cpp)
Thread-safe logger printing steps: `[BUILD]`, `[SKIP]`, `[OK]`, `[FAILED]`, `[BLOCKED]`, `[LINK]`.

---

## Verification Plan

### Automated Tests
We will build a test suite to verify graph operations, configuration parsing, cycle detection, and scheduling correctness:
- Run `ctest` or run a custom test binary.

### Manual Verification
We will verify with a 3-5 file example project under `/Users/shanitsinghal/Desktop/forge/examples/demo`:
1. **Fresh Build**: Verify all files compile and executable is created.
2. **Incremental Build (No Changes)**: Verify subsequent runs skip all tasks (`[SKIP]`).
3. **Source Change**: Modify one `.cpp` source, verify only that source and link steps run.
4. **Header Change**: Modify a common header, verify all files depending on it rebuild.
5. **Cycle Detection**: Introduce `#include` loop in demo files and verify CLI prints error and aborts.
6. **Parallel Speedup**: Build with `-j1` vs `-j4` and measure duration.
