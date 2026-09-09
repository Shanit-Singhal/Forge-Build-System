cmake_minimum_required(VERSION 3.12)
project(Forge CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(src/cli)
include_directories(src/config)
include_directories(src/executor)
include_directories(src/planner)
include_directories(src/analyzer)
include_directories(src/dependency)
include_directories(src/scheduler)
include_directories(src/logging)

# Sources
set(SOURCES
    src/cli/main.cpp
    src/cli/command_parser.cpp
    src/config/config_parser.cpp
    src/executor/process_executor.cpp
    src/planner/build_planner.cpp
    src/planner/dirty_checker.cpp
    src/analyzer/dependency_analyzer.cpp
    src/dependency/dependency_graph.cpp
    src/dependency/cycle_detector.cpp
    src/scheduler/thread_pool.cpp
    src/scheduler/scheduler.cpp
    src/logging/logger.cpp
)

# Executable
add_executable(forge ${SOURCES})

# Target options
target_compile_options(forge PRIVATE -Wall -Wextra -Wpedantic)
