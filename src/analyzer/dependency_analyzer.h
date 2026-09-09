#pragma once
#include "dependency_graph.h"
#include <string>
#include <vector>
#include <unordered_set>

class DependencyAnalyzer {
public:
    // Returns a list of all recursive header dependencies for a given source file
    static std::vector<std::string> get_dependencies(
        const std::string& source_file,
        const std::vector<std::string>& include_dirs);

    // Builds a file-level DependencyGraph mapping files to their direct includes
    static DependencyGraph build_file_graph(
        const std::vector<std::string>& sources,
        const std::vector<std::string>& include_dirs);

private:
    static void parse_file(
        const std::string& file_path,
        const std::vector<std::string>& include_dirs,
        std::unordered_set<std::string>& visited,
        std::vector<std::string>& dependencies);
        
    static std::string resolve_include(
        const std::string& current_file,
        const std::string& include_name,
        const std::vector<std::string>& include_dirs);
};
