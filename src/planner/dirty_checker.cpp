#include "dirty_checker.h"
#include <filesystem>

namespace fs = std::filesystem;

bool DirtyChecker::is_dirty(const BuildTask& task) {
    if (task.output.empty()) {
        return true; // No output file, must execute
    }

    std::error_code ec;
    if (!fs::exists(task.output, ec)) {
        return true; // Output file does not exist -> dirty
    }

    auto output_mtime = fs::last_write_time(task.output, ec);
    if (ec) {
        return true;
    }

    for (const auto& dep : task.dependencies) {
        if (!fs::exists(dep, ec)) {
            // Dependency file does not exist yet -> dirty
            return true;
        }

        auto dep_mtime = fs::last_write_time(dep, ec);
        if (!ec && dep_mtime > output_mtime) {
            return true; // Dependency modified after output -> dirty
        }
    }

    return false; // Output is newer than all dependencies -> up-to-date!
}
