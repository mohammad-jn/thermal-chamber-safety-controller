#pragma once

#include <string>

namespace thermal {

class ConsoleLogger {
public:
    void info(const std::string& message) const;
    void warn(const std::string& message) const;
    void error(const std::string& message) const;
};

} // namespace thermal