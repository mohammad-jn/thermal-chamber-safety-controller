#include "scenarios/ScenarioRunner.hpp"
#include "io/SensorSimulator.hpp"

#include <gtest/gtest.h>
#include <fstream>
#include <string>

using namespace thermal;

namespace {
std::string temp_scenario_path(const std::string& name) {
    return "../scenarios/" + name;
}
}

TEST(ScenarioRunnerTest, DefaultScenarioLoadsExpectedEvents) {
    ScenarioRunner runner;
    std::string error_message;

    ASSERT_TRUE(runner.load_from_file("../scenarios/default_scenario.json", error_message));

    const auto& events = runner.events();

    ASSERT_EQ(events.size(), 3U);
    EXPECT_EQ(events[0].tick_index, 16);
    EXPECT_EQ(events[1].tick_index, 20);
    EXPECT_EQ(events[2].tick_index, 28);
}

TEST(ScenarioRunnerTest, OpenDoorEventChangesSensorState) {
    ScenarioRunner runner;
    std::string error_message;
    ASSERT_TRUE(runner.load_from_file("../scenarios/default_scenario.json", error_message));

    SensorSimulator sensor_simulator;
    runner.apply_events_for_tick(16, sensor_simulator);

    const auto sensors = sensor_simulator.read(25.0);
    EXPECT_TRUE(sensors.door_open);
}

TEST(ScenarioRunnerTest, CloseDoorEventChangesSensorState) {
    ScenarioRunner runner;
    std::string error_message;
    ASSERT_TRUE(runner.load_from_file("../scenarios/default_scenario.json", error_message));

    SensorSimulator sensor_simulator;
    sensor_simulator.set_door_open(true);
    runner.apply_events_for_tick(20, sensor_simulator);

    const auto sensors = sensor_simulator.read(25.0);
    EXPECT_FALSE(sensors.door_open);
}

TEST(ScenarioRunnerTest, SensorFreshFalseEventChangesSensorState) {
    ScenarioRunner runner;
    std::string error_message;
    ASSERT_TRUE(runner.load_from_file("../scenarios/default_scenario.json", error_message));

    SensorSimulator sensor_simulator;
    runner.apply_events_for_tick(28, sensor_simulator);

    const auto sensors = sensor_simulator.read(25.0);
    EXPECT_FALSE(sensors.sensor_fresh);
}

TEST(ScenarioRunnerTest, CustomScenarioLoadsFromJson) {
    const auto path = temp_scenario_path("test_custom_scenario.json");

    {
        std::ofstream out(path);
        out << R"({
  "events": [
    { "tick_index": 5, "type": "SetPowerUnavailable" },
    { "tick_index": 7, "type": "SetPowerAvailable" }
  ]
})";
    }

    ScenarioRunner runner;
    std::string error_message;

    ASSERT_TRUE(runner.load_from_file(path, error_message));
    ASSERT_EQ(runner.events().size(), 2U);
    EXPECT_EQ(runner.events()[0].tick_index, 5);
    EXPECT_EQ(runner.events()[1].tick_index, 7);

    std::remove(path.c_str());
}

TEST(ScenarioRunnerTest, UnknownEventTypeFailsToLoad) {
    const auto path = temp_scenario_path("test_bad_scenario.json");

    {
        std::ofstream out(path);
        out << R"({
  "events": [
    { "tick_index": 5, "type": "UnknownThing" }
  ]
})";
    }

    ScenarioRunner runner;
    std::string error_message;

    EXPECT_FALSE(runner.load_from_file(path, error_message));
    EXPECT_FALSE(error_message.empty());

    std::remove(path.c_str());
}