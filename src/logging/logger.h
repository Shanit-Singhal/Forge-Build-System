#pragma once
#include <string>
#include <mutex>

enum class LogLevel {
    INFO,
    RUNNING,
    SUCCESS,
    SKIP,
    FAILED,
    BLOCKED,
    LINK,
    ERROR
};

class Logger {
public:
    static void log(LogLevel level, const std::string& message);
    static void raw(const std::string& message);

private:
    static std::mutex mutex_;
};
