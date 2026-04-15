#include "app/Application.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

namespace thermal {

Application::Application()
    : config_manager_(),
      state_machine_(),
      chamber_model_(),
      controller_(config_manager_.config()),
      fault_manager_(),
      safety_manager_(),
      sensor_simulator_(),
      actuator_model_(),
      logger_() {}

int Application::run(const std::string& config_path) {
    std::string config_error;
    if (!config_manager_.load_from_file(config_path, config_error)) {
        logger_.error("Failed to load configuration: " + config_error);
        return 1;
    }

    sensor_simulator_.set_ambient_temperature_c(config_manager_.config().ambient_temperature_c);

    logger_.info("Starting Thermal Chamber Safety Controller");
    logger_.info("Using config file: " + config_path);

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

    const double dt_seconds = config_manager_.config().dt_seconds;
    const int total_ticks = config_manager_.config().total_ticks;

    for (int tick = 0; tick < total_ticks; ++tick) {
        step(dt_seconds, tick);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    logger_.info("Simulation complete");
    return 0;
}

void Application::step(double dt_seconds, int tick_index) {
    inject_scenario_events(tick_index);

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
    const auto& config = config_manager_.config();

    if (state_machine_.current_state() == SystemState::Preheat &&
        sensors_after.chamber_temperature_c >= config.stabilization_entry_temperature_c) {
        if (state_machine_.transition_to(SystemState::Stabilizing) == TransitionResult::Success) {
            logger_.info("Transitioned to Stabilizing");
        }
    }

    if (state_machine_.current_state() == SystemState::Stabilizing &&
        sensors_after.chamber_temperature_c >= config.running_min_temperature_c &&
        sensors_after.chamber_temperature_c <= config.running_max_temperature_c) {
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
        << "% Fan=" << (applied_commands.fan_on ? "ON" : "OFF")
        << " Door=" << (sensors_after.door_open ? "OPEN" : "CLOSED")
        << " Power=" << (sensors_after.power_available ? "OK" : "LOST")
        << " Fresh=" << (sensors_after.sensor_fresh ? "YES" : "NO");

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

void Application::inject_scenario_events(int tick_index) {
    if (tick_index == 16) {
        sensor_simulator_.set_door_open(true);
        logger_.warn("Scenario event: door opened");
    }

    if (tick_index == 20) {
        sensor_simulator_.set_door_open(false);
        logger_.info("Scenario event: door closed");
    }

    if (tick_index == 28) {
        sensor_simulator_.set_sensor_fresh(false);
        logger_.warn("Scenario event: sensor freshness lost");
    }
}

} // namespace thermal