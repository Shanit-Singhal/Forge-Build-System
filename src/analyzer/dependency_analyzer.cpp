#include "dependency_analyzer.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

std::vector<std::string> DependencyAnalyzer::get_dependencies(
    const std::string& source_file,
    const std::vector<std::string>& include_dirs) {
    
    std::unordered_set<std::string> visited;
    std::vector<std::string> dependencies;
    
    // The source file itself is considered visited, but not a dependency
    visited.insert(source_file);
    
    parse_file(source_file, include_dirs, visited, dependencies);
    
    return dependencies;
}

DependencyGraph DependencyAnalyzer::build_file_graph(
    const std::vector<std::string>& sources,
    const std::vector<std::string>& include_dirs) {
    
    DependencyGraph graph;
    std::unordered_set<std::string> processed_files;
    std::vector<std::string> to_process = sources;
    
    while (!to_process.empty()) {
        std::string current_file = to_process.back();
        to_process.pop_back();
        
        if (processed_files.count(current_file)) continue;
        processed_files.insert(current_file);
        graph.add_node(current_file);
        
        std::ifstream file(current_file);
        if (!file.is_open()) continue;
        
        std::regex include_regex(R"regex(^\s*#\s*include\s*(?:<([^>]+)>|"([^"]+)"))regex");
        std::string line;
        
        while (std::getline(file, line)) {
            std::smatch match;
            if (std::regex_search(line, match, include_regex)) {
                std::string include_name = match[1].matched ? match[1].str() : match[2].str();
                std::string resolved = resolve_include(current_file, include_name, include_dirs);
                if (!resolved.empty()) {
                    graph.add_edge(current_file, resolved);
                    if (!processed_files.count(resolved)) {
                        to_process.push_back(resolved);
                    }
                }
            }
        }
    }
    
    return graph;
}

std::string DependencyAnalyzer::resolve_include(
    const std::string& current_file,
    const std::string& include_name,
    const std::vector<std::string>& include_dirs) {
    
    // First, check relative to the current file's directory
    fs::path current_dir = fs::path(current_file).parent_path();
    fs::path relative_path = current_dir / include_name;
    if (fs::exists(relative_path)) {
        return relative_path.lexically_normal().string();
    }
    
    // Then, check all include directories
    for (const auto& dir : include_dirs) {
        fs::path inc_path = fs::path(dir) / include_name;
        if (fs::exists(inc_path)) {
            return inc_path.lexically_normal().string();
        }
    }
    
    return ""; // Not found
}

void DependencyAnalyzer::parse_file(
    const std::string& file_path,
    const std::vector<std::string>& include_dirs,
    std::unordered_set<std::string>& visited,
    std::vector<std::string>& dependencies) {
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return; // Cannot open file, ignore
    }
    
    // Regex to match #include "..." or #include <...>
    std::regex include_regex(R"regex(^\s*#\s*include\s*(?:<([^>]+)>|"([^"]+)"))regex");
    std::string line;
    
    while (std::getline(file, line)) {
        std::smatch match;
        if (std::regex_search(line, match, include_regex)) {
            std::string include_name;
            if (match[1].matched) { // <...>
                include_name = match[1].str();
            } else if (match[2].matched) { // "..."
                include_name = match[2].str();
            }
            
            std::string resolved_path = resolve_include(file_path, include_name, include_dirs);
            if (!resolved_path.empty()) {
                if (visited.insert(resolved_path).second) {
                    dependencies.push_back(resolved_path);
                    // Recursively parse the included file
                    parse_file(resolved_path, include_dirs, visited, dependencies);
                }
            }
        }
    }
}
