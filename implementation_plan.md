# VS Code Extension Implementation & Marketplace Publishing Plan for Forge

This plan outlines the design, implementation, packaging (`.vsix`), and step-by-step instructions for publishing the official **Forge Build System** extension to the Visual Studio Code Marketplace.

---

## User Review Required

> [!IMPORTANT]
> **Marketplace Publishing Requirements**:
> To publish to the public VS Code Marketplace, you will need:
> 1. A free **Microsoft Account** / Azure DevOps organization (we provide step-by-step guide below).
> 2. A **Personal Access Token (PAT)** from Azure DevOps to authenticate `vsce publish`.
> 3. A **Publisher Name** (e.g. `shanitsinghal`).
> 
> We will scaffold, code, test, and package the complete extension locally into a `.vsix` file so it is 100% ready for publishing.

---

## Proposed Extension Architecture

The extension will be created under `/Users/shanitsinghal/Desktop/Forge/vscode-extension/`.

### 1. Extension Features & Commands
- **Commands**:
  - `Forge: Build Project` (`forge.build`) -> Runs `forge build` in an Output Channel.
  - `Forge: Parallel Build` (`forge.buildParallel`) -> Prompts user for thread count (`-j N`) and executes build.
  - `Forge: Clean Artifacts` (`forge.clean`) -> Runs `forge clean`.
  - `Forge: Show Dependency Graph` (`forge.graph`) -> Runs `forge graph` and displays dependency tree in output channel.
- **Status Bar Integration**:
  - Interactive status bar item at the bottom of VS Code (`⚙️ Forge: Build` and `📊 Forge: Graph`).
- **Auto-Detection**:
  - Automatically checks if workspace contains a `forge.conf` file.

### 2. Files to Create in `vscode-extension/`

#### [NEW] [package.json](file:///Users/shanitsinghal/Desktop/Forge/vscode-extension/package.json)
Extension manifest defining extension commands, status bar items, categories, keywords, engine versions, and Marketplace publisher details.

#### [NEW] [tsconfig.json](file:///Users/shanitsinghal/Desktop/Forge/vscode-extension/tsconfig.json)
TypeScript compilation configuration.

#### [NEW] [extension.ts](file:///Users/shanitsinghal/Desktop/Forge/vscode-extension/src/extension.ts)
Main extension entry point handling command registrations, VS Code OutputChannel, process execution, and status bar management.

#### [NEW] [.vscodeignore](file:///Users/shanitsinghal/Desktop/Forge/vscode-extension/.vscodeignore)
Excludes development files (`src/*.ts`, `tsconfig.json`) from the published `.vsix` package.

#### [NEW] [README.md](file:///Users/shanitsinghal/Desktop/Forge/vscode-extension/README.md)
Extension documentation displayed on the VS Code Marketplace store page.

---

## Step-by-Step Publishing Workflow

1. **Scaffold & Build Extension**: Create TypeScript source code and build JS bundle.
2. **Local Package Test**: Package into `forge-build-system-0.0.1.vsix` using `@vscode/vsce`.
3. **Local VS Code Test**: Verify installation of `.vsix` in VS Code.
4. **Publish to Marketplace**: Walk through creating Personal Access Token and running `vsce publish`.

---

## Verification Plan

### Automated Build & Packaging
- Run `npm run compile` to build TypeScript code.
- Run `npx @vscode/vsce package` to verify VSIX package generation without errors.

### Manual Verification
- Test command palette execution (`Forge: Build`, `Forge: Clean`, `Forge: Show Dependency Graph`).
- Test status bar button clicks.
