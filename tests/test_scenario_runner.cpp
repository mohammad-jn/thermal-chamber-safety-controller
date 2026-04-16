#include "scenarios/ScenarioRunner.hpp"
#include "io/SensorSimulator.hpp"

#include <gtest/gtest.h>

using namespace thermal;

TEST(ScenarioRunnerTest, DefaultScenarioContainsExpectedEvents) {
    ScenarioRunner runner;

    const auto& events = runner.events();

    ASSERT_EQ(events.size(), 3U);
    EXPECT_EQ(events[0].tick_index, 16);
    EXPECT_EQ(events[1].tick_index, 20);
    EXPECT_EQ(events[2].tick_index, 28);
}

TEST(ScenarioRunnerTest, OpenDoorEventChangesSensorState) {
    ScenarioRunner runner;
    SensorSimulator sensor_simulator;

    runner.apply_events_for_tick(16, sensor_simulator);

    const auto sensors = sensor_simulator.read(25.0);
    EXPECT_TRUE(sensors.door_open);
}

TEST(ScenarioRunnerTest, CloseDoorEventChangesSensorState) {
    ScenarioRunner runner;
    SensorSimulator sensor_simulator;

    sensor_simulator.set_door_open(true);
    runner.apply_events_for_tick(20, sensor_simulator);

    const auto sensors = sensor_simulator.read(25.0);
    EXPECT_FALSE(sensors.door_open);
}

TEST(ScenarioRunnerTest, SensorFreshFalseEventChangesSensorState) {
    ScenarioRunner runner;
    SensorSimulator sensor_simulator;

    runner.apply_events_for_tick(28, sensor_simulator);

    const auto sensors = sensor_simulator.read(25.0);
    EXPECT_FALSE(sensors.sensor_fresh);
}