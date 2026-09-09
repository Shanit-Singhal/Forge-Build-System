#pragma once
#include "build_task.h"
#include <string>
#include <vector>

class DirtyChecker {
public:
    // Returns true if the output file is missing or if any dependency is newer than the output file
    static bool is_dirty(const BuildTask& task);
};
