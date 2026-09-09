# Forge Build System VS Code Extension

Official VS Code extension for **Forge** - a high-performance C++ dependency-aware mini build system.

GitHub Repository: [https://github.com/Shanit-Singhal/Forge-Build-System](https://github.com/Shanit-Singhal/Forge-Build-System)

---

## How to Use This Extension in Any C++ Project

### Step 1: Install Forge CLI Binary (One-Time Setup)
To let Forge run from any folder, copy the `forge` binary executable to `/usr/local/bin`:
```bash
sudo cp build/forge /usr/local/bin/
```
*(Now `forge` is accessible globally from any terminal directory).*

### Step 2: Add a `forge.conf` to Your C++ Project Root
In any external project folder (e.g. `~/Desktop/my_cpp_project`), create a file named `forge.conf`:
```ini
compiler = clang++
flags = -std=c++17 -Wall
sources = src/main.cpp src/math.cpp
includes = include
output = build/my_program
```

### Step 3: Run Extension Commands inside VS Code
Open your project folder in VS Code. The extension automatically detects `forge.conf`. You can now:
- Click **`[⚙️ Forge: Build]`** or **`[📊 Forge: Graph]`** on the bottom status bar.
- Open Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`) and type `Forge: Build Project`.

---

## Features

- **Integrated Build Pipeline**: Execute `forge build` directly inside VS Code with real-time colored output logs.
- **Multithreaded Parallel Builds**: Specify thread count (`-j N`) for parallel compilation tasks.
- **Clean Artifacts**: Quickly remove compiled `.o` object files and target executables (`forge clean`).
- **Dependency Graph Tree**: Visualize your C++ project `#include` dependency graph as an ASCII tree (`forge graph`).
- **Status Bar Integration**: Convenient quick-action status bar buttons (`⚙️ Forge: Build` and `📊 Forge: Graph`).

---

## Extension Commands

Access these commands from the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`):

| Command | Title | Description |
| :--- | :--- | :--- |
| `forge.build` | **Forge: Build Project** | Compiles out-of-date C++ source files and links target executable. |
| `forge.buildParallel` | **Forge: Build Parallel (-j)** | Prompts for thread count and executes parallel task compilation. |
| `forge.clean` | **Forge: Clean Artifacts** | Cleans build directory and output binaries. |
| `forge.graph` | **Forge: Show Dependency Graph** | Displays ASCII dependency tree of source files and headers. |
