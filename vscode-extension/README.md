# Forge Build System VS Code Extension

Official VS Code extension for **Forge** - a high-performance C++ dependency-aware mini build system.

---

## Features

- **Integrated Build Pipeline**: Execute `forge build` directly inside VS Code with real-time colored output logs.
- **Multithreaded Parallel Builds**: Specify thread count (`-j N`) for parallel compilation tasks.
- **Clean Artifacts**: Quickly remove compiled `.o` object files and target executables (`forge clean`).
- **Dependency Graph Tree**: Visualize your C++ project `#include` dependency graph as an ASCII tree (`forge graph`).
- **Status Bar Integration**: Convenient quick-action status bar buttons (`⚙️ Forge: Build` and `📊 Forge: Graph`).
- **Auto-Detection**: Automatically detects workspaces containing `forge.conf`.

---

## Extension Commands

Access these commands from the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`):

| Command | Title | Description |
| :--- | :--- | :--- |
| `forge.build` | **Forge: Build Project** | Compiles out-of-date C++ source files and links target executable. |
| `forge.buildParallel` | **Forge: Build Parallel (-j)** | Prompts for thread count and executes parallel task compilation. |
| `forge.clean` | **Forge: Clean Artifacts** | Cleans build directory and output binaries. |
| `forge.graph` | **Forge: Show Dependency Graph** | Displays ASCII dependency tree of source files and headers. |

---

## Prerequisites

Ensure the `forge` build executable is available on your system PATH (e.g. `/usr/local/bin/forge`) or located inside your workspace's `./build/forge` directory.

---

## Sample `forge.conf` Project Setup

Create a `forge.conf` file in your workspace root:

```ini
compiler = clang++
flags = -std=c++17 -Wall -O2
sources = src/main.cpp src/math.cpp src/utils.cpp
includes = include
output = build/my_program
```
