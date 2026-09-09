#include "config.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace {

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::vector<std::string> split_spaces(const std::string& str) {
    std::vector<std::string> result;
    std::string current;
    for (char c : str) {
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

} // namespace

bool parse_config(const std::string& filepath, BuildConfig& out_config, std::string& out_error) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        out_error = "Could not open configuration file: " + filepath;
        return false;
    }

    std::string line;
    int line_number = 0;

    // Track which keys were parsed for validation
    bool has_compiler = false;
    bool has_sources = false;
    bool has_output = false;

    while (std::getline(file, line)) {
        line_number++;

        // Remove comments starting with '#'
        size_t hash_pos = line.find('#');
        if (hash_pos != std::string::npos) {
            line = line.substr(0, hash_pos);
        }

        line = trim(line);
        if (line.empty()) {
            continue;
        }

        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) {
            out_error = "Invalid configuration format at line " + std::to_string(line_number) + ": expected 'key = value'";
            return false;
        }

        std::string key = trim(line.substr(0, eq_pos));
        std::string val = trim(line.substr(eq_pos + 1));

        if (key == "compiler") {
            out_config.compiler = val;
            has_compiler = !val.empty();
        } else if (key == "flags") {
            out_config.flags = val;
        } else if (key == "sources") {
            out_config.sources = split_spaces(val);
            has_sources = !out_config.sources.empty();
        } else if (key == "includes") {
            out_config.includes = split_spaces(val);
        } else if (key == "output") {
            out_config.output = val;
            has_output = !val.empty();
        } else {
            out_error = "Unknown configuration key at line " + std::to_string(line_number) + ": '" + key + "'";
            return false;
        }
    }

    // Validation
    if (!has_compiler) {
        out_error = "Configuration error: missing or empty required field 'compiler'";
        return false;
    }
    if (!has_sources) {
        out_error = "Configuration error: missing or empty required field 'sources'";
        return false;
    }
    if (!has_output) {
        out_error = "Configuration error: missing or empty required field 'output'";
        return false;
    }

    return true;
}
