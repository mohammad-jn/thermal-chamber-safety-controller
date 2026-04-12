#include "app/Application.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

namespace thermal {

int Application::run() {
    logger_.info("Starting Thermal Chamber Safety Controller");

    state_machine_.transition_to(SystemState::SelfTest);
    logger_.info("Transitioned to SelfTest");

    state_machine_.transition_to(SystemState::Idle);
    logger_.info("Transitioned to Idle");

    state_machine_.transition_to(SystemState::Preheat);
    logger_.info("Transitioned to Preheat");

    constexpr double dt_seconds = 0.5;
    constexpr int total_ticks = 40;

    for (int tick = 0; tick < total_ticks; ++tick) {
        step(dt_seconds, tick);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    logger_.info("Simulation complete");
    return 0;
}

void Application::step(double dt_seconds, int tick_index) {
    apply_phase1_control_logic();

    const auto commands = actuator_model_.current_commands();
    const auto sensors_before = sensor_simulator_.read(chamber_model_.chamber_temperature_c());

    chamber_model_.update(
        dt_seconds,
        commands.heater_power_pct,
        commands.cooler_power_pct,
        commands.fan_on,
        sensors_before.ambient_temperature_c
    );

    const auto sensors_after = sensor_simulator_.read(chamber_model_.chamber_temperature_c());

    if (state_machine_.current_state() == SystemState::Preheat &&
        sensors_after.chamber_temperature_c >= 68.0) {
        if (state_machine_.transition_to(SystemState::Stabilizing) == TransitionResult::Success) {
            logger_.info("Transitioned to Stabilizing");
        }
    }

    if (state_machine_.current_state() == SystemState::Stabilizing &&
        sensors_after.chamber_temperature_c >= 70.0 &&
        sensors_after.chamber_temperature_c <= 76.0) {
        if (state_machine_.transition_to(SystemState::Running) == TransitionResult::Success) {
            logger_.info("Transitioned to Running");
        }
    }

    std::ostringstream oss;
    oss << "Tick=" << tick_index
        << " State=" << to_string(state_machine_.current_state())
        << " TempC=" << std::fixed << std::setprecision(2) << sensors_after.chamber_temperature_c
        << " Heater=" << commands.heater_power_pct
        << "% Cooler=" << commands.cooler_power_pct
        << "% Fan=" << (commands.fan_on ? "ON" : "OFF");

    logger_.info(oss.str());
}

void Application::apply_phase1_control_logic() {
    ActuatorCommands commands {};

    switch (state_machine_.current_state()) {
        case SystemState::Preheat:
            commands.heater_power_pct = 85.0;
            commands.cooler_power_pct = 0.0;
            commands.fan_on = true;
            break;

        case SystemState::Stabilizing:
        case SystemState::Running: {
            const double current_temp = chamber_model_.chamber_temperature_c();
            const double setpoint = 75.0;

            if (current_temp < setpoint - 1.0) {
                commands.heater_power_pct = 35.0;
                commands.cooler_power_pct = 0.0;
            } else if (current_temp > setpoint + 1.0) {
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

    actuator_model_.set_commands(commands);
}

} // namespace thermal