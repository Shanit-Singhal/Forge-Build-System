#pragma once

#include <string>
#include <vector>

struct ProcessResult {
    bool success;
    int exit_code;
    std::string output; // Combined stdout and stderr
};

class ProcessExecutor {
public:
    // Execute a command with its arguments.
    // Returns a ProcessResult containing success status, exit code, and captured output.
    static ProcessResult execute(const std::vector<std::string>& command);
};
