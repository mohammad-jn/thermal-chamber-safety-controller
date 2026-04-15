#pragma once

#include "core/Types.hpp"

#include <string>

namespace thermal {

class ConfigManager {
public:
    ConfigManager();

    [[nodiscard]] const SimulationConfig& config() const noexcept;
    [[nodiscard]] bool validate(std::string& error_message) const noexcept;

    [[nodiscard]] bool load_from_file(
        const std::string& file_path,
        std::string& error_message
    ) noexcept;

private:
    SimulationConfig config_;
};

} // namespace thermal