#pragma once

#include "core/Types.hpp"

namespace thermal {

class Controller {
public:
    explicit Controller(const SimulationConfig& config);

    [[nodiscard]] ActuatorCommands compute_commands(
        SystemState state,
        double current_temperature_c
    ) const noexcept;

private:
    const SimulationConfig& config_;
};

} // namespace thermal