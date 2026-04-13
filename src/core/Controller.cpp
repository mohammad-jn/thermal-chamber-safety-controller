#include "core/Controller.hpp"

namespace thermal {

ActuatorCommands Controller::compute_commands(
    SystemState state,
    double current_temperature_c
) const noexcept {
    ActuatorCommands commands {};

    switch (state) {
        case SystemState::Preheat:
            commands.heater_power_pct = 85.0;
            commands.cooler_power_pct = 0.0;
            commands.fan_on = true;
            break;

        case SystemState::Stabilizing:
        case SystemState::Running: {
            constexpr double setpoint_c = 75.0;

            if (current_temperature_c < setpoint_c - 1.0) {
                commands.heater_power_pct = 55.0;
                commands.cooler_power_pct = 0.0;
            } else if (current_temperature_c > setpoint_c + 1.0) {
                commands.heater_power_pct = 0.0;
                commands.cooler_power_pct = 30.0;
            } else {
                commands.heater_power_pct = 10.0;
                commands.cooler_power_pct = 0.0;
            }

            commands.fan_on = true;
            break;
        }

        default:
            commands.heater_power_pct = 0.0;
            commands.cooler_power_pct = 0.0;
            commands.fan_on = false;
            break;
    }

    return commands;
}

} // namespace thermal