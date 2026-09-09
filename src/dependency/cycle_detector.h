#pragma once
#include "dependency_graph.h"
#include <string>
#include <vector>

class CycleDetector {
public:
    // Returns true if a cycle exists in the graph.
    // If a cycle is found, cycle_path is populated with the sequence of nodes forming the cycle.
    static bool detect_cycle(const DependencyGraph& graph, std::vector<std::string>& cycle_path);
};
