#include "command_parser.h"
#include "config.h"
#include "process_executor.h"
#include "build_planner.h"
#include "build_task.h"
#include "dependency_graph.h"
#include "cycle_detector.h"
#include "dependency_analyzer.h"
#include "dirty_checker.h"
#include "scheduler.h"
#include "logger.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    ParsedArgs args = parse_arguments(argc, argv);

    if (!args.success) {
        Logger::log(LogLevel::ERROR, args.error_message);
        print_help();
        return 1;
    }

    if (args.cmd == Command::HELP) {
        print_help();
        return 0;
    }

    // Load configuration for all main commands (build, clean, graph)
    BuildConfig config;
    std::string config_error;
    std::string config_path = "forge.conf";

    if (!parse_config(config_path, config, config_error)) {
        Logger::log(LogLevel::ERROR, "Failed to load config '" + config_path + "': " + config_error);
        return 1;
    }

    switch (args.cmd) {
        case Command::BUILD: {
            Logger::log(LogLevel::INFO, "Building project (threads: " + std::to_string(args.thread_count) + ", keep-going: " + (args.keep_going ? "true" : "false") + ")");
            
            std::vector<BuildTask> tasks = BuildPlanner::create_tasks(config);
            Logger::log(LogLevel::INFO, std::to_string(tasks.size()) + " tasks discovered.");
            
            // Build file dependency graph for cycle detection
            DependencyGraph graph = DependencyAnalyzer::build_file_graph(config.sources, config.includes);
            
            std::vector<std::string> cycle_path;
            if (CycleDetector::detect_cycle(graph, cycle_path)) {
                std::string cycle_str;
                for (size_t i = 0; i < cycle_path.size(); ++i) {
                    cycle_str += cycle_path[i];
                    if (i + 1 < cycle_path.size()) cycle_str += " -> ";
                }
                Logger::log(LogLevel::ERROR, "Circular dependency detected: " + cycle_str);
                return 1;
            }
            
            bool build_success = Scheduler::execute_build(tasks, args.thread_count, args.keep_going);
            if (build_success) {
                Logger::log(LogLevel::SUCCESS, "Build completed successfully.");
            } else {
                Logger::log(LogLevel::ERROR, "Build failed.");
                return 1;
            }
            break;
        }
        case Command::CLEAN: {
            Logger::log(LogLevel::INFO, "Cleaning build artifacts...");
            size_t removed_count = 0;
            std::error_code ec;

            // Remove output target
            if (fs::exists(config.output, ec)) {
                fs::remove(config.output, ec);
                removed_count++;
            }

            // Remove object files
            fs::path output_path(config.output);
            std::string build_dir = output_path.parent_path().string();
            if (build_dir.empty()) build_dir = ".";

            for (const auto& src : config.sources) {
                fs::path src_path(src);
                std::string obj_file = build_dir + "/" + src_path.filename().string() + ".o";
                if (fs::exists(obj_file, ec)) {
                    fs::remove(obj_file, ec);
                    removed_count++;
                }
            }

            Logger::log(LogLevel::SUCCESS, "Cleaned " + std::to_string(removed_count) + " build artifact(s).");
            break;
        }
        case Command::GRAPH: {
            Logger::log(LogLevel::INFO, "Project Dependency Graph (" + config.output + ")");
            DependencyGraph graph = DependencyAnalyzer::build_file_graph(config.sources, config.includes);
            graph.print_ascii_tree(config.sources);
            break;
        }
        default:
            std::cout << "unknown\n";
            break;
    }

    std::cout << "[INFO] Configuration loaded successfully:\n";
    std::cout << "  Compiler: " << config.compiler << "\n";
    std::cout << "  Flags:    " << config.flags << "\n";
    std::cout << "  Sources:  ";
    for (const auto& src : config.sources) {
        std::cout << src << " ";
    }
    std::cout << "\n  Includes: ";
    for (const auto& inc : config.includes) {
        std::cout << inc << " ";
    }
    std::cout << "\n  Output:   " << config.output << "\n";

    return 0;
}
