#pragma once

#include <string>
#include <vector>

namespace thermal {

enum class SystemState {
    Startup,
    SelfTest,
    Idle,
    Preheat,
    Stabilizing,
    Running,
    Warning,
    Fault,
    EmergencyShutdown
};

enum class TransitionResult {
    Success,
    InvalidTransition
};

enum class FaultSeverity {
    Warning,
    Critical
};

enum class FaultType {
    PowerLoss,
    SensorStale,
    DoorOpenDuringActiveHeating,
    OverTemperatureWarning,
    OverTemperatureCritical
};

struct FaultRecord {
    FaultType type;
    FaultSeverity severity;
    std::string message;
};

struct SimulationConfig {
    double target_temperature_c {75.0};
    double stabilization_entry_temperature_c {68.0};
    double running_min_temperature_c {70.0};
    double running_max_temperature_c {76.0};

    double overtemperature_warning_c {85.0};
    double overtemperature_critical_c {95.0};

    double preheat_heater_power_pct {85.0};
    double control_heater_high_pct {55.0};
    double control_heater_low_pct {10.0};
    double control_cooler_pct {30.0};

    double control_band_c {1.0};

    double ambient_temperature_c {22.0};
    double dt_seconds {0.5};
    int total_ticks {40};
};

inline std::string to_string(SystemState state) {
    switch (state) {
        case SystemState::Startup: return "Startup";
        case SystemState::SelfTest: return "SelfTest";
        case SystemState::Idle: return "Idle";
        case SystemState::Preheat: return "Preheat";
        case SystemState::Stabilizing: return "Stabilizing";
        case SystemState::Running: return "Running";
        case SystemState::Warning: return "Warning";
        case SystemState::Fault: return "Fault";
        case SystemState::EmergencyShutdown: return "EmergencyShutdown";
        default: return "Unknown";
    }
}

inline std::string to_string(FaultSeverity severity) {
    switch (severity) {
        case FaultSeverity::Warning: return "Warning";
        case FaultSeverity::Critical: return "Critical";
        default: return "Unknown";
    }
}

inline std::string to_string(FaultType type) {
    switch (type) {
        case FaultType::PowerLoss: return "PowerLoss";
        case FaultType::SensorStale: return "SensorStale";
        case FaultType::DoorOpenDuringActiveHeating: return "DoorOpenDuringActiveHeating";
        case FaultType::OverTemperatureWarning: return "OverTemperatureWarning";
        case FaultType::OverTemperatureCritical: return "OverTemperatureCritical";
        default: return "Unknown";
    }
}

struct SensorReadings {
    double chamber_temperature_c {25.0};
    double ambient_temperature_c {22.0};
    bool door_open {false};
    bool airflow_ok {true};
    bool power_available {true};
    bool sensor_fresh {true};
};

struct ActuatorCommands {
    double heater_power_pct {0.0};
    double cooler_power_pct {0.0};
    bool fan_on {false};
};

using FaultList = std::vector<FaultRecord>;

} // namespace thermal