#include "diagnostics/ConsoleLogger.hpp"

#include <iostream>

namespace thermal {

void ConsoleLogger::info(const std::string& message) const {
    std::cout << "[INFO] " << message << '\n';
}

void ConsoleLogger::warn(const std::string& message) const {
    std::cout << "[WARN] " << message << '\n';
}

void ConsoleLogger::error(const std::string& message) const {
    std::cerr << "[ERROR] " << message << '\n';
}

} // namespace thermal