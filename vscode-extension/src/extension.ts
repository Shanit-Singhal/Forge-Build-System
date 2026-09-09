import * as vscode from 'vscode';
import { exec } from 'child_process';
import * as fs from 'fs';
import * as path from 'path';

let outputChannel: vscode.OutputChannel;
let statusBarBuild: vscode.StatusBarItem;
let statusBarGraph: vscode.StatusBarItem;

function getForgePath(cwd?: string): string {
    if (cwd) {
        const localForge = path.join(cwd, 'build', 'forge');
        if (fs.existsSync(localForge)) {
            return localForge;
        }
    }
    return 'forge'; // Fallback to system PATH
}

function runForgeCommand(args: string) {
    const workspaceFolders = vscode.workspace.workspaceFolders;
    if (!workspaceFolders || workspaceFolders.length === 0) {
        vscode.window.showErrorMessage("Forge: No workspace folder open.");
        return;
    }

    const cwd = workspaceFolders[0].uri.fsPath;
    const forgeBin = getForgePath(cwd);
    const fullCmd = `"${forgeBin}" ${args}`;

    if (!outputChannel) {
        outputChannel = vscode.window.createOutputChannel("Forge Build System");
    }

    outputChannel.show(true);
    outputChannel.appendLine(`\n> Running: ${fullCmd} (in ${cwd})`);

    exec(fullCmd, { cwd }, (error, stdout, stderr) => {
        if (stdout) {
            outputChannel.appendLine(stdout);
        }
        if (stderr) {
            outputChannel.appendLine(stderr);
        }

        if (error) {
            vscode.window.showErrorMessage(`Forge Command Failed: ${error.message}`);
        } else {
            vscode.window.showInformationMessage(`Forge: Command '${args}' completed successfully.`);
        }
    });
}

export function activate(context: vscode.ExtensionContext) {
    outputChannel = vscode.window.createOutputChannel("Forge Build System");

    // Status Bar Items
    statusBarBuild = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
    statusBarBuild.command = 'forge.build';
    statusBarBuild.text = '$(tools) Forge: Build';
    statusBarBuild.tooltip = 'Click to run Forge Build';
    statusBarBuild.show();

    statusBarGraph = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 99);
    statusBarGraph.command = 'forge.graph';
    statusBarGraph.text = '$(graph) Forge: Graph';
    statusBarGraph.tooltip = 'Click to show Forge Dependency Graph';
    statusBarGraph.show();

    context.subscriptions.push(statusBarBuild);
    context.subscriptions.push(statusBarGraph);

    // Register Commands
    const buildCmd = vscode.commands.registerCommand('forge.build', () => {
        runForgeCommand('build');
    });

    const buildParallelCmd = vscode.commands.registerCommand('forge.buildParallel', async () => {
        const threadCount = await vscode.window.showInputBox({
            prompt: "Enter number of parallel worker threads (-j)",
            value: "4"
        });
        if (threadCount) {
            runForgeCommand(`build -j ${threadCount}`);
        }
    });

    const cleanCmd = vscode.commands.registerCommand('forge.clean', () => {
        runForgeCommand('clean');
    });

    const graphCmd = vscode.commands.registerCommand('forge.graph', () => {
        runForgeCommand('graph');
    });

    context.subscriptions.push(buildCmd, buildParallelCmd, cleanCmd, graphCmd);
}

export function deactivate() {
    if (outputChannel) {
        outputChannel.dispose();
    }
}
