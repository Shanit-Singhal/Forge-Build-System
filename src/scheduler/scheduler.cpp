#include "scheduler.h"
#include "dirty_checker.h"
#include "process_executor.h"
#include "thread_pool.h"
#include "logger.h"
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <queue>

namespace fs = std::filesystem;

bool Scheduler::execute_build(
    std::vector<BuildTask>& tasks,
    int thread_count,
    bool keep_going) {

    if (tasks.empty()) return true;

    // Output file mapping to task index
    std::unordered_map<std::string, size_t> output_to_task_idx;
    for (size_t i = 0; i < tasks.size(); ++i) {
        if (!tasks[i].output.empty()) {
            output_to_task_idx[tasks[i].output] = i;
        }
    }

    // Build dependents list & remaining_deps counters
    std::vector<std::vector<size_t>> dependents(tasks.size());
    std::vector<int> remaining_deps(tasks.size(), 0);

    for (size_t i = 0; i < tasks.size(); ++i) {
        for (const auto& dep_file : tasks[i].dependencies) {
            auto it = output_to_task_idx.find(dep_file);
            if (it != output_to_task_idx.end()) {
                size_t dep_task_idx = it->second;
                dependents[dep_task_idx].push_back(i);
                remaining_deps[i]++;
            }
        }
    }

    std::mutex mtx;
    std::condition_variable cv;
    size_t completed_count = 0;
    bool has_failed = false;

    // Check clean tasks and propagate
    std::queue<size_t> clean_queue;
    for (size_t i = 0; i < tasks.size(); ++i) {
        if (!DirtyChecker::is_dirty(tasks[i])) {
            tasks[i].state = TaskState::SUCCESS;
            Logger::log(LogLevel::SKIP, tasks[i].name + " (up to date)");
            clean_queue.push(i);
        }
    }

    while (!clean_queue.empty()) {
        size_t idx = clean_queue.front();
        clean_queue.pop();
        completed_count++;
        for (size_t dep_idx : dependents[idx]) {
            remaining_deps[dep_idx]--;
        }
    }

    if (completed_count == tasks.size()) {
        return true; // All tasks were up to date
    }

    ThreadPool pool(thread_count > 0 ? thread_count : 1);

    // Recursively dispatch ready tasks
    std::function<void(size_t)> dispatch_task = [&](size_t task_idx) {
        pool.enqueue([&, task_idx]() {
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (has_failed && !keep_going) {
                    tasks[task_idx].state = TaskState::BLOCKED;
                    Logger::log(LogLevel::BLOCKED, tasks[task_idx].name);
                    completed_count++;
                    cv.notify_all();
                    return;
                }
                tasks[task_idx].state = TaskState::RUNNING;
            }

            if (tasks[task_idx].name.rfind("Link", 0) == 0) {
                Logger::log(LogLevel::LINK, tasks[task_idx].name);
            } else {
                Logger::log(LogLevel::RUNNING, tasks[task_idx].name);
            }

            ProcessResult result = ProcessExecutor::execute(tasks[task_idx].command);

            std::unique_lock<std::mutex> lock(mtx);
            if (result.success) {
                tasks[task_idx].state = TaskState::SUCCESS;
                Logger::log(LogLevel::SUCCESS, tasks[task_idx].name);

                for (size_t dep_idx : dependents[task_idx]) {
                    remaining_deps[dep_idx]--;
                    if (remaining_deps[dep_idx] == 0 && tasks[dep_idx].state == TaskState::PENDING) {
                        lock.unlock();
                        dispatch_task(dep_idx);
                        lock.lock();
                    }
                }
            } else {
                tasks[task_idx].state = TaskState::FAILED;
                has_failed = true;

                // Cleanup partial output file on failure
                if (!tasks[task_idx].output.empty()) {
                    std::error_code ec;
                    fs::remove(tasks[task_idx].output, ec);
                }

                Logger::log(LogLevel::FAILED, tasks[task_idx].name + " (exit code " + std::to_string(result.exit_code) + ")\n" + result.output);
            }

            completed_count++;
            cv.notify_all();
        });
    };

    // Dispatch initial ready tasks
    {
        std::lock_guard<std::mutex> lock(mtx);
        for (size_t i = 0; i < tasks.size(); ++i) {
            if (remaining_deps[i] == 0 && tasks[i].state == TaskState::PENDING) {
                dispatch_task(i);
            }
        }
    }

    // Wait until all tasks finish
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]() {
        return completed_count >= tasks.size();
    });

    return !has_failed;
}
