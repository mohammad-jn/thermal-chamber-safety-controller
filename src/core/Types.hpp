#pragma once

#include <string>

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

} // namespace thermal