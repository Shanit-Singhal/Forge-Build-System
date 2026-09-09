#include "build_planner.h"
#include "dependency_analyzer.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

std::vector<BuildTask> BuildPlanner::create_tasks(const BuildConfig& config) {
    std::vector<BuildTask> tasks;
    std::vector<std::string> object_files;
    
    // Determine build directory from output
    fs::path output_path(config.output);
    std::string build_dir = output_path.parent_path().string();
    if (build_dir.empty()) {
        build_dir = ".";
    }

    // Ensure output directory exists
    std::error_code ec;
    fs::create_directories(build_dir, ec);

    // Split flags
    std::vector<std::string> flags;
    std::string current;
    for (char c : config.flags) {
        if (c == ' ' || c == '\t') {
            if (!current.empty()) {
                flags.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) {
        flags.push_back(current);
    }
    
    // Compile tasks
    for (const auto& src : config.sources) {
        BuildTask task;
        task.name = "Compile " + src;
        
        fs::path src_path(src);
        std::string obj_file = build_dir + "/" + src_path.filename().string() + ".o";
        object_files.push_back(obj_file);
        
        task.output = obj_file;
        
        // Use DependencyAnalyzer to get header dependencies
        task.dependencies = DependencyAnalyzer::get_dependencies(src, config.includes);
        // The source file itself is also a dependency
        task.dependencies.push_back(src);
        
        task.command.push_back(config.compiler);
        for (const auto& flag : flags) {
            task.command.push_back(flag);
        }
        for (const auto& inc : config.includes) {
            task.command.push_back("-I" + inc);
        }
        task.command.push_back("-c");
        task.command.push_back(src);
        task.command.push_back("-o");
        task.command.push_back(obj_file);
        
        tasks.push_back(task);
    }
    
    // Link task
    BuildTask link_task;
    link_task.name = "Link " + config.output;
    link_task.output = config.output;
    link_task.dependencies = object_files;
    
    link_task.command.push_back(config.compiler);
    for (const auto& flag : flags) {
        link_task.command.push_back(flag);
    }
    for (const auto& obj : object_files) {
        link_task.command.push_back(obj);
    }
    link_task.command.push_back("-o");
    link_task.command.push_back(config.output);
    
    tasks.push_back(link_task);
    
    return tasks;
}
