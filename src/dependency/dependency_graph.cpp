#include "dependency_graph.h"
#include <queue>
#include <iostream>

void DependencyGraph::add_node(const std::string& node) {
    nodes_.insert(node);
    if (adj_list_.find(node) == adj_list_.end()) {
        adj_list_[node] = {};
    }
}

void DependencyGraph::add_edge(const std::string& node, const std::string& dependency) {
    add_node(node);
    add_node(dependency);
    adj_list_[node].push_back(dependency);
}

const std::unordered_map<std::string, std::vector<std::string>>& DependencyGraph::get_adjacency_list() const {
    return adj_list_;
}

const std::unordered_set<std::string>& DependencyGraph::get_nodes() const {
    return nodes_;
}

std::vector<std::string> DependencyGraph::topological_sort() const {
    std::unordered_map<std::string, int> in_degree;
    for (const auto& node : nodes_) {
        in_degree[node] = 0;
    }

    for (const auto& [node, neighbors] : adj_list_) {
        for (const auto& neighbor : neighbors) {
            in_degree[neighbor]++;
        }
    }

    std::queue<std::string> q;
    for (const auto& node : nodes_) {
        if (in_degree[node] == 0) {
            q.push(node);
        }
    }

    std::vector<std::string> result;
    while (!q.empty()) {
        std::string current = q.front();
        q.pop();
        result.push_back(current);

        auto it = adj_list_.find(current);
        if (it != adj_list_.end()) {
            for (const auto& neighbor : it->second) {
                in_degree[neighbor]--;
                if (in_degree[neighbor] == 0) {
                    q.push(neighbor);
                }
            }
        }
    }

    return result;
}

void DependencyGraph::print_ascii_tree(const std::vector<std::string>& root_sources) const {
    for (size_t i = 0; i < root_sources.size(); ++i) {
        bool is_last_src = (i + 1 == root_sources.size());
        std::string src_prefix = is_last_src ? "└── " : "├── ";
        std::string child_indent = is_last_src ? "    " : "│   ";
        
        std::cout << src_prefix << root_sources[i] << "\n";
        
        auto it = adj_list_.find(root_sources[i]);
        if (it != adj_list_.end() && !it->second.empty()) {
            for (size_t j = 0; j < it->second.size(); ++j) {
                bool is_last_dep = (j + 1 == it->second.size());
                std::string dep_prefix = is_last_dep ? "└── " : "├── ";
                std::cout << child_indent << dep_prefix << it->second[j] << "\n";
            }
        }
    }
}
