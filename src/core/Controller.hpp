#pragma once

#include "core/Types.hpp"

namespace thermal {

class Controller {
public:
    Controller() = default;

    [[nodiscard]] ActuatorCommands compute_commands(
        SystemState state,
        double current_temperature_c
    ) const noexcept;
};

} // namespace thermal