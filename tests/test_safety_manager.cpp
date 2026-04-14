#include "core/SafetyManager.hpp"
#include "core/Types.hpp"

#include <gtest/gtest.h>

using namespace thermal;

TEST(SafetyManagerTest, AllowsRequestedCommandsInNormalConditions) {
    SafetyManager safety_manager;

    SensorReadings sensors {};
    sensors.door_open = false;
    sensors.power_available = true;
    sensors.sensor_fresh = true;

    ActuatorCommands requested {};
    requested.heater_power_pct = 50.0;
    requested.cooler_power_pct = 0.0;
    requested.fan_on = true;

    const auto applied = safety_manager.apply_interlocks(
        SystemState::Running,
        sensors,
        requested
    );

    EXPECT_DOUBLE_EQ(applied.heater_power_pct, 50.0);
    EXPECT_DOUBLE_EQ(applied.cooler_power_pct, 0.0);
    EXPECT_TRUE(applied.fan_on);
}

TEST(SafetyManagerTest, BlocksHeaterWhenDoorIsOpen) {
    SafetyManager safety_manager;

    SensorReadings sensors {};
    sensors.door_open = true;
    sensors.power_available = true;
    sensors.sensor_fresh = true;

    ActuatorCommands requested {};
    requested.heater_power_pct = 60.0;
    requested.cooler_power_pct = 0.0;
    requested.fan_on = true;

    const auto applied = safety_manager.apply_interlocks(
        SystemState::Running,
        sensors,
        requested
    );

    EXPECT_DOUBLE_EQ(applied.heater_power_pct, 0.0);
    EXPECT_DOUBLE_EQ(applied.cooler_power_pct, 0.0);
    EXPECT_TRUE(applied.fan_on);
}

TEST(SafetyManagerTest, ShutsEverythingOffWhenPowerIsUnavailable) {
    SafetyManager safety_manager;

    SensorReadings sensors {};
    sensors.door_open = false;
    sensors.power_available = false;
    sensors.sensor_fresh = true;

    ActuatorCommands requested {};
    requested.heater_power_pct = 60.0;
    requested.cooler_power_pct = 20.0;
    requested.fan_on = true;

    const auto applied = safety_manager.apply_interlocks(
        SystemState::Running,
        sensors,
        requested
    );

    EXPECT_DOUBLE_EQ(applied.heater_power_pct, 0.0);
    EXPECT_DOUBLE_EQ(applied.cooler_power_pct, 0.0);
    EXPECT_FALSE(applied.fan_on);
}

TEST(SafetyManagerTest, ShutsEverythingOffWhenSensorDataIsStale) {
    SafetyManager safety_manager;

    SensorReadings sensors {};
    sensors.door_open = false;
    sensors.power_available = true;
    sensors.sensor_fresh = false;

    ActuatorCommands requested {};
    requested.heater_power_pct = 60.0;
    requested.cooler_power_pct = 20.0;
    requested.fan_on = true;

    const auto applied = safety_manager.apply_interlocks(
        SystemState::Running,
        sensors,
        requested
    );

    EXPECT_DOUBLE_EQ(applied.heater_power_pct, 0.0);
    EXPECT_DOUBLE_EQ(applied.cooler_power_pct, 0.0);
    EXPECT_FALSE(applied.fan_on);
}

TEST(SafetyManagerTest, FaultStateShutsEverythingOff) {
    SafetyManager safety_manager;

    SensorReadings sensors {};
    sensors.door_open = false;
    sensors.power_available = true;
    sensors.sensor_fresh = true;

    ActuatorCommands requested {};
    requested.heater_power_pct = 60.0;
    requested.cooler_power_pct = 20.0;
    requested.fan_on = true;

    const auto applied = safety_manager.apply_interlocks(
        SystemState::Fault,
        sensors,
        requested
    );

    EXPECT_DOUBLE_EQ(applied.heater_power_pct, 0.0);
    EXPECT_DOUBLE_EQ(applied.cooler_power_pct, 0.0);
    EXPECT_FALSE(applied.fan_on);
}

TEST(SafetyManagerTest, EmergencyShutdownShutsEverythingOff) {
    SafetyManager safety_manager;

    SensorReadings sensors {};
    sensors.door_open = false;
    sensors.power_available = true;
    sensors.sensor_fresh = true;

    ActuatorCommands requested {};
    requested.heater_power_pct = 60.0;
    requested.cooler_power_pct = 20.0;
    requested.fan_on = true;

    const auto applied = safety_manager.apply_interlocks(
        SystemState::EmergencyShutdown,
        sensors,
        requested
    );

    EXPECT_DOUBLE_EQ(applied.heater_power_pct, 0.0);
    EXPECT_DOUBLE_EQ(applied.cooler_power_pct, 0.0);
    EXPECT_FALSE(applied.fan_on);
}