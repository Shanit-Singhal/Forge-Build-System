#include "command_parser.h"
#include <iostream>
#include <string>
#include <vector>

void print_help() {
    std::cout << "Forge - Mini Dependency-Aware Build System\n\n"
              << "Usage:\n"
              << "  forge <command> [options]\n\n"
              << "Commands:\n"
              << "  build       Build the project specified in forge.conf\n"
              << "  clean       Clean the build directory and output targets\n"
              << "  graph       Visualize the project's dependency graph\n"
              << "  help        Display this help message\n\n"
              << "Options:\n"
              << "  -j <threads>      Number of parallel worker threads (default: 1)\n"
              << "  -k, --keep-going  Keep building independent targets after a failure\n"
              << "  -h, --help        Display this help message\n";
}

ParsedArgs parse_arguments(int argc, char* argv[]) {
    ParsedArgs parsed;
    if (argc < 2) {
        parsed.cmd = Command::HELP;
        return parsed;
    }

    std::string command_str = argv[1];
    if (command_str == "build") {
        parsed.cmd = Command::BUILD;
    } else if (command_str == "clean") {
        parsed.cmd = Command::CLEAN;
    } else if (command_str == "graph") {
        parsed.cmd = Command::GRAPH;
    } else if (command_str == "help" || command_str == "-h" || command_str == "--help") {
        parsed.cmd = Command::HELP;
        return parsed;
    } else {
        parsed.success = false;
        parsed.error_message = "Unknown command: " + command_str;
        return parsed;
    }

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-j") {
            if (i + 1 >= argc) {
                parsed.success = false;
                parsed.error_message = "-j option requires an integer argument for thread count";
                return parsed;
            }
            std::string thread_str = argv[++i];
            try {
                int threads = std::stoi(thread_str);
                if (threads <= 0) {
                    parsed.success = false;
                    parsed.error_message = "Thread count must be a positive integer: " + thread_str;
                    return parsed;
                }
                parsed.thread_count = threads;
            } catch (const std::exception&) {
                parsed.success = false;
                parsed.error_message = "Invalid thread count argument: " + thread_str;
                return parsed;
            }
        } else if (arg == "-k" || arg == "--keep-going") {
            parsed.keep_going = true;
        } else if (arg == "-h" || arg == "--help") {
            parsed.cmd = Command::HELP;
            return parsed;
        } else {
            parsed.success = false;
            parsed.error_message = "Unknown option: " + arg;
            return parsed;
        }
    }

    return parsed;
}
