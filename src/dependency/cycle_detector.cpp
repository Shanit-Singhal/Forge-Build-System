#include "cycle_detector.h"
#include <unordered_map>
#include <algorithm>

namespace {
enum Color { WHITE = 0, GRAY = 1, BLACK = 2 };

bool dfs(
    const std::string& current,
    const std::unordered_map<std::string, std::vector<std::string>>& adj_list,
    std::unordered_map<std::string, Color>& color,
    std::unordered_map<std::string, std::string>& parent,
    std::string& cycle_start,
    std::string& cycle_end) {

    color[current] = GRAY;

    auto it = adj_list.find(current);
    if (it != adj_list.end()) {
        for (const auto& neighbor : it->second) {
            if (color[neighbor] == GRAY) {
                // Cycle detected!
                cycle_start = neighbor;
                cycle_end = current;
                return true;
            }
            if (color[neighbor] == WHITE) {
                parent[neighbor] = current;
                if (dfs(neighbor, adj_list, color, parent, cycle_start, cycle_end)) {
                    return true;
                }
            }
        }
    }

    color[current] = BLACK;
    return false;
}
}

bool CycleDetector::detect_cycle(const DependencyGraph& graph, std::vector<std::string>& cycle_path) {
    cycle_path.clear();
    const auto& adj_list = graph.get_adjacency_list();
    const auto& nodes = graph.get_nodes();

    std::unordered_map<std::string, Color> color;
    std::unordered_map<std::string, std::string> parent;

    for (const auto& node : nodes) {
        color[node] = WHITE;
    }

    std::string cycle_start;
    std::string cycle_end;

    for (const auto& node : nodes) {
        if (color[node] == WHITE) {
            if (dfs(node, adj_list, color, parent, cycle_start, cycle_end)) {
                // Reconstruct cycle path from cycle_end back to cycle_start
                cycle_path.push_back(cycle_start);
                std::string curr = cycle_end;
                while (curr != cycle_start && !curr.empty()) {
                    cycle_path.push_back(curr);
                    curr = parent[curr];
                }
                cycle_path.push_back(cycle_start);
                std::reverse(cycle_path.begin(), cycle_path.end());
                return true;
            }
        }
    }

    return false;
}
