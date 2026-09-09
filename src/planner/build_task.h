#pragma once
#include <string>
#include <vector>

enum class TaskState { PENDING, READY, RUNNING, SUCCESS, FAILED, BLOCKED };

struct BuildTask {
    std::string name;
    std::vector<std::string> command;
    std::string output;
    std::vector<std::string> dependencies;
    TaskState state = TaskState::PENDING;
};
