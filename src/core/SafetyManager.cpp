#include "core/SafetyManager.hpp"

namespace thermal {

ActuatorCommands SafetyManager::apply_interlocks(
    SystemState state,
    const SensorReadings& sensors,
    const ActuatorCommands& requested_commands
) const noexcept {
    ActuatorCommands safe_commands = requested_commands;

    if (state == SystemState::Fault || state == SystemState::EmergencyShutdown) {
        safe_commands.heater_power_pct = 0.0;
        safe_commands.cooler_power_pct = 0.0;
        safe_commands.fan_on = false;
        return safe_commands;
    }

    if (!sensors.power_available || !sensors.sensor_fresh) {
        safe_commands.heater_power_pct = 0.0;
        safe_commands.cooler_power_pct = 0.0;
        safe_commands.fan_on = false;
        return safe_commands;
    }

    if (sensors.door_open) {
        safe_commands.heater_power_pct = 0.0;
    }

    return safe_commands;
}

} // namespace thermal