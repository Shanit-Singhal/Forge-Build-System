# Forge - Mini Dependency-Aware Build System

Forge is a high-performance build system written in C++17. It automates building C/C++ projects with dynamic header dependency discovery, cycle detection, incremental rebuilds, and parallel task execution.

---

## Building Forge

To build the `forge` executable from source:

```bash
cd /Users/shanitsinghal/Desktop/Forge
cmake -S . -B build
cmake --build build
```

### Optional: Running Forge Globally
If you would like to run `forge` directly from any folder without specifying the path, add its build directory to your PATH or copy it to `/usr/local/bin`:

```bash
sudo cp /Users/shanitsinghal/Desktop/Forge/build/forge /usr/local/bin/
```
Then you can simply type `forge build`, `forge clean`, or `forge graph` in any project folder containing a `forge.conf` file!

---

## How to Use Forge

### 1. Create a `forge.conf` in your C++ project root

To use Forge with any C++ project, create a configuration file named `forge.conf` in your project's root directory:

```ini
# Compiler binary
compiler = clang++

# Compiler flags
flags = -std=c++17 -Wall -O2

# Space-separated list of source files
sources = src/main.cpp src/math.cpp src/utils.cpp

# Space-separated list of include directories
includes = include

# Target output executable path
output = build/my_program
```

---

### 2. Available Commands

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

### 3. Useful Commands for Future Updates
When you make changes or add features to your extension in the future:

- **Publish a minor update / bug fix (v0.0.2)**:
  ```bash
  npx @vscode/vsce publish patch
  ```
- **Publish a feature update (v0.1.0)**:
  ```bash
  npx @vscode/vsce publish minor
  ```
- **Publish a major release (v1.0.0)**:
  ```bash
  npx @vscode/vsce publish major
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
