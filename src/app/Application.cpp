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
    const auto sensors_before = sensor_simulator_.read(chamber_model_.chamber_temperature_c());

    const auto requested_commands = controller_.compute_commands(
        state_machine_.current_state(),
        chamber_model_.chamber_temperature_c()
    );

    const auto safe_commands = safety_manager_.apply_interlocks(
        state_machine_.current_state(),
        sensors_before,
        requested_commands
    );

    actuator_model_.set_commands(safe_commands);
    const auto applied_commands = actuator_model_.current_commands();

    const auto faults_before = fault_manager_.detect_faults(sensors_before, applied_commands);
    log_faults(faults_before);

    if (fault_manager_.has_critical_fault(faults_before) &&
        state_machine_.current_state() != SystemState::Fault &&
        state_machine_.current_state() != SystemState::EmergencyShutdown) {
        if (state_machine_.transition_to(SystemState::Fault) == TransitionResult::Success) {
            logger_.error("Transitioned to Fault due to critical fault condition");
        }
    } else if (fault_manager_.has_warning_fault(faults_before) &&
               state_machine_.current_state() == SystemState::Running) {
        if (state_machine_.transition_to(SystemState::Warning) == TransitionResult::Success) {
            logger_.warn("Transitioned to Warning due to warning fault condition");
        }
    }

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

void Application::log_faults(const FaultList& faults) {
    for (const auto& fault : faults) {
        std::ostringstream oss;
        oss << "Fault detected: " << to_string(fault.type)
            << " Severity=" << to_string(fault.severity)
            << " Message=" << fault.message;

        if (fault.severity == FaultSeverity::Critical) {
            logger_.error(oss.str());
        } else {
            logger_.warn(oss.str());
        }
    }
}

} // namespace thermal