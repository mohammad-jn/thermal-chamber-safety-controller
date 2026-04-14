#include "core/FaultManager.hpp"
#include "core/Types.hpp"

#include <gtest/gtest.h>

using namespace thermal;

TEST(FaultManagerTest, DetectsNoFaultsInNormalConditions) {
    FaultManager fault_manager;

    SensorReadings sensors {};
    sensors.chamber_temperature_c = 70.0;
    sensors.power_available = true;
    sensors.sensor_fresh = true;
    sensors.door_open = false;

    ActuatorCommands commands {};
    commands.heater_power_pct = 40.0;
    commands.cooler_power_pct = 0.0;
    commands.fan_on = true;

    const auto faults = fault_manager.detect_faults(sensors, commands);

    EXPECT_TRUE(faults.empty());
}

TEST(FaultManagerTest, DetectsPowerLossAsCritical) {
    FaultManager fault_manager;

    SensorReadings sensors {};
    sensors.power_available = false;

    ActuatorCommands commands {};

    const auto faults = fault_manager.detect_faults(sensors, commands);

    ASSERT_EQ(faults.size(), 1U);
    EXPECT_EQ(faults[0].type, FaultType::PowerLoss);
    EXPECT_EQ(faults[0].severity, FaultSeverity::Critical);
}

TEST(FaultManagerTest, DetectsSensorStaleAsCritical) {
    FaultManager fault_manager;

    SensorReadings sensors {};
    sensors.sensor_fresh = false;

    ActuatorCommands commands {};

    const auto faults = fault_manager.detect_faults(sensors, commands);

    ASSERT_EQ(faults.size(), 1U);
    EXPECT_EQ(faults[0].type, FaultType::SensorStale);
    EXPECT_EQ(faults[0].severity, FaultSeverity::Critical);
}

TEST(FaultManagerTest, DetectsDoorOpenDuringHeatingAsCritical) {
    FaultManager fault_manager;

    SensorReadings sensors {};
    sensors.door_open = true;

    ActuatorCommands commands {};
    commands.heater_power_pct = 30.0;

    const auto faults = fault_manager.detect_faults(sensors, commands);

    ASSERT_EQ(faults.size(), 1U);
    EXPECT_EQ(faults[0].type, FaultType::DoorOpenDuringActiveHeating);
    EXPECT_EQ(faults[0].severity, FaultSeverity::Critical);
}

TEST(FaultManagerTest, DetectsOverTemperatureWarning) {
    FaultManager fault_manager;

    SensorReadings sensors {};
    sensors.chamber_temperature_c = 86.0;

    ActuatorCommands commands {};

    const auto faults = fault_manager.detect_faults(sensors, commands);

    ASSERT_EQ(faults.size(), 1U);
    EXPECT_EQ(faults[0].type, FaultType::OverTemperatureWarning);
    EXPECT_EQ(faults[0].severity, FaultSeverity::Warning);
}

TEST(FaultManagerTest, DetectsOverTemperatureCritical) {
    FaultManager fault_manager;

    SensorReadings sensors {};
    sensors.chamber_temperature_c = 96.0;

    ActuatorCommands commands {};

    const auto faults = fault_manager.detect_faults(sensors, commands);

    ASSERT_EQ(faults.size(), 1U);
    EXPECT_EQ(faults[0].type, FaultType::OverTemperatureCritical);
    EXPECT_EQ(faults[0].severity, FaultSeverity::Critical);
}

TEST(FaultManagerTest, ReportsCriticalFaultPresence) {
    FaultManager fault_manager;

    FaultList faults {
        FaultRecord{FaultType::PowerLoss, FaultSeverity::Critical, "power lost"},
        FaultRecord{FaultType::OverTemperatureWarning, FaultSeverity::Warning, "temp warning"}
    };

    EXPECT_TRUE(fault_manager.has_critical_fault(faults));
    EXPECT_TRUE(fault_manager.has_warning_fault(faults));
}

TEST(FaultManagerTest, ReportsNoCriticalFaultWhenOnlyWarningsExist) {
    FaultManager fault_manager;

    FaultList faults {
        FaultRecord{FaultType::OverTemperatureWarning, FaultSeverity::Warning, "temp warning"}
    };

    EXPECT_FALSE(fault_manager.has_critical_fault(faults));
    EXPECT_TRUE(fault_manager.has_warning_fault(faults));
}