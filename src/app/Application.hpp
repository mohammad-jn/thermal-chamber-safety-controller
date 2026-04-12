#pragma once

#include "core/ChamberModel.hpp"
#include "core/StateMachine.hpp"
#include "diagnostics/ConsoleLogger.hpp"
#include "io/ActuatorModel.hpp"
#include "io/SensorSimulator.hpp"

namespace thermal {

class Application {
public:
    Application() = default;
    int run();

private:
    void step(double dt_seconds, int tick_index);
    void apply_phase1_control_logic();

    StateMachine state_machine_;
    ChamberModel chamber_model_;
    SensorSimulator sensor_simulator_;
    ActuatorModel actuator_model_;
    ConsoleLogger logger_;
};

} // namespace thermal