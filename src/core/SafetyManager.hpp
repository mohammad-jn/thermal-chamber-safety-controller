#pragma once

#include "core/Types.hpp"

namespace thermal {

class SafetyManager {
public:
    SafetyManager() = default;

    [[nodiscard]] ActuatorCommands apply_interlocks(
        SystemState state,
        const SensorReadings& sensors,
        const ActuatorCommands& requested_commands
    ) const noexcept;
};

} // namespace thermal