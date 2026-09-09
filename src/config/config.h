#pragma once

#include <string>
#include <vector>

struct BuildConfig {
    std::string compiler;
    std::string flags;
    std::vector<std::string> sources;
    std::vector<std::string> includes;
    std::string output;
};

// Parses the configuration file at the given filepath.
// Returns true on success, false on failure (setting out_error with the message).
bool parse_config(const std::string& filepath, BuildConfig& out_config, std::string& out_error);
