#pragma once
#include "build_task.h"
#include <vector>

class Scheduler {
public:
    static bool execute_build(
        std::vector<BuildTask>& tasks,
        int thread_count,
        bool keep_going);
};
