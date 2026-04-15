#include "core/Controller.hpp"

namespace thermal {

Controller::Controller(const SimulationConfig& config)
    : config_(config) {}

ActuatorCommands Controller::compute_commands(
    SystemState state,
    double current_temperature_c
) const noexcept {
    ActuatorCommands commands {};

    switch (state) {
        case SystemState::Preheat:
            commands.heater_power_pct = config_.preheat_heater_power_pct;
            commands.cooler_power_pct = 0.0;
            commands.fan_on = true;
            break;

        case SystemState::Stabilizing:
        case SystemState::Running: {
            if (current_temperature_c < config_.target_temperature_c - config_.control_band_c) {
                commands.heater_power_pct = config_.control_heater_high_pct;
                commands.cooler_power_pct = 0.0;
            } else if (current_temperature_c > config_.target_temperature_c + config_.control_band_c) {
                commands.heater_power_pct = 0.0;
                commands.cooler_power_pct = config_.control_cooler_pct;
            } else {
                commands.heater_power_pct = config_.control_heater_low_pct;
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