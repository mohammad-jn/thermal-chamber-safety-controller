#pragma once

#include "core/Types.hpp"

#include <string>

namespace thermal {

class ConfigManager {
public:
    ConfigManager();

    [[nodiscard]] const SimulationConfig& config() const noexcept;
    [[nodiscard]] bool validate(std::string& error_message) const noexcept;

private:
    SimulationConfig config_;
};

} // namespace thermal