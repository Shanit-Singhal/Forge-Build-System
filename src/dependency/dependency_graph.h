#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class DependencyGraph {
public:
    // Adds a directed edge: node depends on dependency (node -> dependency)
    void add_edge(const std::string& node, const std::string& dependency);
    
    // Ensures a node exists in the graph even if it has no dependencies
    void add_node(const std::string& node);

    const std::unordered_map<std::string, std::vector<std::string>>& get_adjacency_list() const;
    const std::unordered_set<std::string>& get_nodes() const;

    // Returns nodes sorted in topological order using Kahn's algorithm
    std::vector<std::string> topological_sort() const;

    // Prints ASCII tree representation of dependency graph starting from root sources
    void print_ascii_tree(const std::vector<std::string>& root_sources) const;

private:
    std::unordered_set<std::string> nodes_;
    std::unordered_map<std::string, std::vector<std::string>> adj_list_; // node -> list of dependencies
};
