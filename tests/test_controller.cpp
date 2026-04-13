#include "core/Controller.hpp"
#include "core/Types.hpp"

#include <gtest/gtest.h>

using namespace thermal;

TEST(ControllerTest, PreheatUsesStrongHeatingAndFan) {
    Controller controller;

    const auto commands = controller.compute_commands(SystemState::Preheat, 30.0);

    EXPECT_DOUBLE_EQ(commands.heater_power_pct, 85.0);
    EXPECT_DOUBLE_EQ(commands.cooler_power_pct, 0.0);
    EXPECT_TRUE(commands.fan_on);
}

TEST(ControllerTest, StabilizingHeatsWhenBelowSetpointBand) {
    Controller controller;

    const auto commands = controller.compute_commands(SystemState::Stabilizing, 70.0);

    EXPECT_DOUBLE_EQ(commands.heater_power_pct, 55.0);
    EXPECT_DOUBLE_EQ(commands.cooler_power_pct, 0.0);
    EXPECT_TRUE(commands.fan_on);
}

TEST(ControllerTest, RunningUsesLowHeatNearSetpoint) {
    Controller controller;

    const auto commands = controller.compute_commands(SystemState::Running, 75.0);

    EXPECT_DOUBLE_EQ(commands.heater_power_pct, 10.0);
    EXPECT_DOUBLE_EQ(commands.cooler_power_pct, 0.0);
    EXPECT_TRUE(commands.fan_on);
}

TEST(ControllerTest, RunningUsesCoolingAboveSetpointBand) {
    Controller controller;

    const auto commands = controller.compute_commands(SystemState::Running, 77.0);

    EXPECT_DOUBLE_EQ(commands.heater_power_pct, 0.0);
    EXPECT_DOUBLE_EQ(commands.cooler_power_pct, 30.0);
    EXPECT_TRUE(commands.fan_on);
}

TEST(ControllerTest, IdleTurnsOutputsOff) {
    Controller controller;

    const auto commands = controller.compute_commands(SystemState::Idle, 25.0);

    EXPECT_DOUBLE_EQ(commands.heater_power_pct, 0.0);
    EXPECT_DOUBLE_EQ(commands.cooler_power_pct, 0.0);
    EXPECT_FALSE(commands.fan_on);
}

TEST(ControllerTest, FaultTurnsOutputsOff) {
    Controller controller;

    const auto commands = controller.compute_commands(SystemState::Fault, 90.0);

    EXPECT_DOUBLE_EQ(commands.heater_power_pct, 0.0);
    EXPECT_DOUBLE_EQ(commands.cooler_power_pct, 0.0);
    EXPECT_FALSE(commands.fan_on);
}