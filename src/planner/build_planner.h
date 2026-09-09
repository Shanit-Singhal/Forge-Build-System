#pragma once
#include "config.h"
#include "build_task.h"
#include <vector>

class BuildPlanner {
public:
    static std::vector<BuildTask> create_tasks(const BuildConfig& config);
};
