#include "core/ChamberModel.hpp"
#include "core/Controller.hpp"
#include "core/FaultManager.hpp"
#include "core/SafetyManager.hpp"
#include "core/StateMachine.hpp"
#include "core/Types.hpp"
#include "io/ActuatorModel.hpp"
#include "io/SensorSimulator.hpp"

#include <gtest/gtest.h>

using namespace thermal;

namespace {

void run_single_step(
    StateMachine& state_machine,
    ChamberModel& chamber_model,
    Controller& controller,
    SafetyManager& safety_manager,
    FaultManager& fault_manager,
    SensorSimulator& sensor_simulator,
    ActuatorModel& actuator_model,
    double dt_seconds
) {
    const auto sensors_before = sensor_simulator.read(chamber_model.chamber_temperature_c());

    const auto requested_commands = controller.compute_commands(
        state_machine.current_state(),
        chamber_model.chamber_temperature_c()
    );

    const auto safe_commands = safety_manager.apply_interlocks(
        state_machine.current_state(),
        sensors_before,
        requested_commands
    );

    actuator_model.set_commands(safe_commands);
    const auto applied_commands = actuator_model.current_commands();

    const auto faults = fault_manager.detect_faults(sensors_before, applied_commands);

    if (fault_manager.has_critical_fault(faults) &&
        state_machine.current_state() != SystemState::Fault &&
        state_machine.current_state() != SystemState::EmergencyShutdown) {
        state_machine.transition_to(SystemState::Fault);
    } else if (fault_manager.has_warning_fault(faults) &&
               state_machine.current_state() == SystemState::Running) {
        state_machine.transition_to(SystemState::Warning);
    }

    chamber_model.update(
        dt_seconds,
        applied_commands.heater_power_pct,
        applied_commands.cooler_power_pct,
        applied_commands.fan_on,
        sensors_before.ambient_temperature_c
    );

    const auto sensors_after = sensor_simulator.read(chamber_model.chamber_temperature_c());

    if (state_machine.current_state() == SystemState::Preheat &&
        sensors_after.chamber_temperature_c >= 68.0) {
        state_machine.transition_to(SystemState::Stabilizing);
    }

    if (state_machine.current_state() == SystemState::Stabilizing &&
        sensors_after.chamber_temperature_c >= 70.0 &&
        sensors_after.chamber_temperature_c <= 76.0) {
        state_machine.transition_to(SystemState::Running);
    }
}

} // namespace

TEST(IntegrationScenarioTest, NormalStartupReachesRunning) {
    StateMachine state_machine;
    ChamberModel chamber_model;
    Controller controller;
    SafetyManager safety_manager;
    FaultManager fault_manager;
    SensorSimulator sensor_simulator;
    ActuatorModel actuator_model;

    constexpr double dt_seconds = 0.5;

    ASSERT_EQ(state_machine.transition_to(SystemState::SelfTest), TransitionResult::Success);
    ASSERT_EQ(state_machine.transition_to(SystemState::Idle), TransitionResult::Success);
    ASSERT_EQ(state_machine.transition_to(SystemState::Preheat), TransitionResult::Success);

    for (int i = 0; i < 40; ++i) {
        run_single_step(
            state_machine,
            chamber_model,
            controller,
            safety_manager,
            fault_manager,
            sensor_simulator,
            actuator_model,
            dt_seconds
        );

        if (state_machine.current_state() == SystemState::Running) {
            break;
        }
    }

    EXPECT_EQ(state_machine.current_state(), SystemState::Running);
}

TEST(IntegrationScenarioTest, DoorOpenBlocksHeater) {
    StateMachine state_machine;
    ChamberModel chamber_model;
    Controller controller;
    SafetyManager safety_manager;
    FaultManager fault_manager;
    SensorSimulator sensor_simulator;
    ActuatorModel actuator_model;

    constexpr double dt_seconds = 0.5;

    ASSERT_EQ(state_machine.transition_to(SystemState::SelfTest), TransitionResult::Success);
    ASSERT_EQ(state_machine.transition_to(SystemState::Idle), TransitionResult::Success);
    ASSERT_EQ(state_machine.transition_to(SystemState::Preheat), TransitionResult::Success);

    sensor_simulator.set_door_open(true);

    run_single_step(
        state_machine,
        chamber_model,
        controller,
        safety_manager,
        fault_manager,
        sensor_simulator,
        actuator_model,
        dt_seconds
    );

    const auto applied = actuator_model.current_commands();
    EXPECT_DOUBLE_EQ(applied.heater_power_pct, 0.0);
    EXPECT_TRUE(applied.fan_on);
}

TEST(IntegrationScenarioTest, SensorStaleTransitionsSystemToFault) {
    StateMachine state_machine;
    ChamberModel chamber_model;
    Controller controller;
    SafetyManager safety_manager;
    FaultManager fault_manager;
    SensorSimulator sensor_simulator;
    ActuatorModel actuator_model;

    constexpr double dt_seconds = 0.5;

    ASSERT_EQ(state_machine.transition_to(SystemState::SelfTest), TransitionResult::Success);
    ASSERT_EQ(state_machine.transition_to(SystemState::Idle), TransitionResult::Success);
    ASSERT_EQ(state_machine.transition_to(SystemState::Preheat), TransitionResult::Success);

    sensor_simulator.set_sensor_fresh(false);

    run_single_step(
        state_machine,
        chamber_model,
        controller,
        safety_manager,
        fault_manager,
        sensor_simulator,
        actuator_model,
        dt_seconds
    );

    EXPECT_EQ(state_machine.current_state(), SystemState::Fault);

    const auto applied = actuator_model.current_commands();
    EXPECT_DOUBLE_EQ(applied.heater_power_pct, 0.0);
    EXPECT_DOUBLE_EQ(applied.cooler_power_pct, 0.0);
    EXPECT_FALSE(applied.fan_on);
}