# Forge - Mini Dependency-Aware Build System

Forge is a high-performance build system written in C++17. It automates building C/C++ projects with dynamic header dependency discovery, cycle detection, incremental rebuilds, and parallel task execution.

GitHub Repository: [https://github.com/Shanit-Singhal/Forge-Build-System](https://github.com/Shanit-Singhal/Forge-Build-System)

---

## Building Forge

To build the `forge` executable from source:

```bash
cd /Users/shanitsinghal/Desktop/Forge
cmake -S . -B build
cmake --build build
```

### Step 1: Install the forge CLI executable on your system PATH (One-time setup)

To let Forge run from any folder, copy the `forge` binary to `/usr/local/bin`:

```bash
sudo cp build/forge /usr/local/bin/
```
(Now `forge` is accessible globally from any terminal directory).

### Step 2: Add a `forge.conf` to ANY C++ project directory

In any external project folder (e.g. `~/Desktop/my_cpp_project`), create a file named `forge.conf`:

```ini
compiler = clang++
flags = -std=c++17 -Wall
sources = src/main.cpp src/math.cpp
includes = include
output = build/my_program
```

---

## How to Use Forge

### Available Commands

#### Build Project (`forge build`)
Compiles out-of-date source files and links the final executable:
```bash
forge build
```

#### Parallel Build (`-j <threads>`)
Specify the number of concurrent worker threads for faster parallel compilation:
```bash
forge build -j 4
```

#### Keep-Going Mode (`-k` / `--keep-going`)
Continue compiling independent branches of the dependency graph even if one compilation fails:
```bash
forge build -j 4 -k
```

#### Visualize Dependency Graph (`forge graph`)
Prints an ASCII tree showing source files and their detected header dependencies:
```bash
forge graph
```

#### Clean Build Artifacts (`forge clean`)
Deletes object files (`.o`) and the compiled target executable:
```bash
forge clean
```

---

## Command Options Reference

| Command | Option | Description |
| :--- | :--- | :--- |
| `build` | `-j <N>` | Run compilation tasks in parallel across `N` worker threads. |
| `build` | `-k` | Keep going after compilation errors, building independent targets. |
| `clean` | — | Remove target executable and all object files. |
| `graph` | — | Print ASCII dependency tree of sources and included headers. |

---

## VS Code Extension: How to Run, Package & Publish

Forge includes an official VS Code extension located under `vscode-extension/`.

### 1. Build and Test Locally
```bash
cd /Users/shanitsinghal/Desktop/Forge/vscode-extension
npm run compile
npx @vscode/vsce package --no-git-tag-version
```
To install locally into VS Code / Cursor:
```bash
code --install-extension forge-build-system-0.0.1.vsix
```

### 2. Publish to VS Code Marketplace
Log in with your publisher name (e.g., `ShanitSinghal`) and Personal Access Token (PAT):
```bash
cd /Users/shanitsinghal/Desktop/Forge/vscode-extension
npx @vscode/vsce login ShanitSinghal
npx @vscode/vsce publish
```

---

## Project Architecture & Directory Structure

```
/Users/shanitsinghal/Desktop/Forge/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── cli/             # Command-line argument parsing & main entry point
│   ├── config/          # forge.conf key-value parser
│   ├── executor/        # POSIX process runner (fork/execvp/waitpid)
│   ├── planner/         # Task creation & last_write_time dirty checking
│   ├── analyzer/        # Recursive #include header scanner
│   ├── dependency/      # Dependency DAG & 3-color DFS cycle detector
│   ├── scheduler/       # Multithreaded task queue & thread pool
│   └── logging/         # Thread-safe ANSI colored logger
├── vscode-extension/    # Official VS Code Extension & VSIX package
└── examples/            # Example test projects (demo, cycle_demo, fail_demo)
```
