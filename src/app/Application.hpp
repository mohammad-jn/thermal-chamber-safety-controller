#pragma once

#include "config/ConfigManager.hpp"
#include "core/ChamberModel.hpp"
#include "core/Controller.hpp"
#include "core/FaultManager.hpp"
#include "core/SafetyManager.hpp"
#include "core/StateMachine.hpp"
#include "diagnostics/ConsoleLogger.hpp"
#include "io/ActuatorModel.hpp"
#include "io/SensorSimulator.hpp"

namespace thermal {

class Application {
public:
    Application();
    int run();

private:
    void step(double dt_seconds, int tick_index);
    void log_faults(const FaultList& faults);
    void inject_scenario_events(int tick_index);

    ConfigManager config_manager_;
    StateMachine state_machine_;
    ChamberModel chamber_model_;
    Controller controller_;
    FaultManager fault_manager_;
    SafetyManager safety_manager_;
    SensorSimulator sensor_simulator_;
    ActuatorModel actuator_model_;
    ConsoleLogger logger_;
};

} // namespace thermal