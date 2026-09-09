#include "logger.h"
#include <iostream>

std::mutex Logger::mutex_;

namespace {
const char* RESET = "\033[0m";
const char* RED = "\033[31m";
const char* GREEN = "\033[32m";
const char* YELLOW = "\033[33m";
const char* BLUE = "\033[34m";
const char* MAGENTA = "\033[35m";
const char* CYAN = "\033[36m";
const char* BOLD = "\033[1m";
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    switch (level) {
        case LogLevel::INFO:
            std::cout << BOLD << CYAN << "[INFO] " << RESET << message << "\n";
            break;
        case LogLevel::RUNNING:
            std::cout << BOLD << YELLOW << "[RUNNING] " << RESET << message << "\n";
            break;
        case LogLevel::SUCCESS:
            std::cout << BOLD << GREEN << "[OK] " << RESET << message << "\n";
            break;
        case LogLevel::SKIP:
            std::cout << BOLD << BLUE << "[SKIP] " << RESET << message << "\n";
            break;
        case LogLevel::FAILED:
            std::cout << BOLD << RED << "[FAILED] " << RESET << message << "\n";
            break;
        case LogLevel::BLOCKED:
            std::cout << BOLD << MAGENTA << "[BLOCKED] " << RESET << message << "\n";
            break;
        case LogLevel::LINK:
            std::cout << BOLD << CYAN << "[LINK] " << RESET << message << "\n";
            break;
        case LogLevel::ERROR:
            std::cerr << BOLD << RED << "[ERROR] " << RESET << message << "\n";
            break;
    }
}

void Logger::raw(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << message << std::flush;
}
