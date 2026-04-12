#include "io/ActuatorModel.hpp"

#include <algorithm>

namespace thermal {

void ActuatorModel::set_commands(const ActuatorCommands& commands) {
    commands_.heater_power_pct = std::clamp(commands.heater_power_pct, 0.0, 100.0);
    commands_.cooler_power_pct = std::clamp(commands.cooler_power_pct, 0.0, 100.0);
    commands_.fan_on = commands.fan_on;
}

const ActuatorCommands& ActuatorModel::current_commands() const noexcept {
    return commands_;
}

} // namespace thermal