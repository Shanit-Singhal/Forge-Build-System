#pragma once

#include <string>

enum class Command {
    BUILD,
    CLEAN,
    GRAPH,
    HELP
};

struct ParsedArgs {
    Command cmd = Command::HELP;
    int thread_count = 1;      // Default to 1, -j specifies it
    bool keep_going = false;   // -k specifies it
    bool success = true;
    std::string error_message;
};

ParsedArgs parse_arguments(int argc, char* argv[]);
void print_help();
