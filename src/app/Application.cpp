#include "app/Application.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

namespace thermal {

int Application::run() {
    logger_.info("Starting Thermal Chamber Safety Controller");

    if (state_machine_.transition_to(SystemState::SelfTest) == TransitionResult::Success) {
        logger_.info("Transitioned to SelfTest");
    } else {
        logger_.error("Failed to transition to SelfTest");
        return 1;
    }

    if (state_machine_.transition_to(SystemState::Idle) == TransitionResult::Success) {
        logger_.info("Transitioned to Idle");
    } else {
        logger_.error("Failed to transition to Idle");
        return 1;
    }

    if (state_machine_.transition_to(SystemState::Preheat) == TransitionResult::Success) {
        logger_.info("Transitioned to Preheat");
    } else {
        logger_.error("Failed to transition to Preheat");
        return 1;
    }

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
    const auto commands = controller_.compute_commands(
        state_machine_.current_state(),
        chamber_model_.chamber_temperature_c()
    );

    actuator_model_.set_commands(commands);

    const auto applied_commands = actuator_model_.current_commands();
    const auto sensors_before = sensor_simulator_.read(chamber_model_.chamber_temperature_c());

    chamber_model_.update(
        dt_seconds,
        applied_commands.heater_power_pct,
        applied_commands.cooler_power_pct,
        applied_commands.fan_on,
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
        << " Heater=" << applied_commands.heater_power_pct
        << "% Cooler=" << applied_commands.cooler_power_pct
        << "% Fan=" << (applied_commands.fan_on ? "ON" : "OFF");

    logger_.info(oss.str());
}

} // namespace thermal