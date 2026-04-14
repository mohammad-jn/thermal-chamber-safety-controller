#pragma once

#include "core/Types.hpp"

namespace thermal {

class FaultManager {
public:
    FaultManager() = default;

    [[nodiscard]] FaultList detect_faults(
        const SensorReadings& sensors,
        const ActuatorCommands& applied_commands
    ) const;

    [[nodiscard]] bool has_critical_fault(const FaultList& faults) const noexcept;
    [[nodiscard]] bool has_warning_fault(const FaultList& faults) const noexcept;
};

} // namespace thermal