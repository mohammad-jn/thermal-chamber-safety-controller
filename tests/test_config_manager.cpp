#include "config/ConfigManager.hpp"

#include <gtest/gtest.h>
#include <fstream>
#include <string>

using namespace thermal;

namespace {
std::string default_config_path() {
    return "../config/default_config.json";
}

std::string temp_test_config_path(const std::string& name) {
    return "../config/" + name;
}
}

TEST(ConfigManagerTest, DefaultConfigIsValid) {
    ConfigManager manager;

    std::string error_message;
    ASSERT_TRUE(manager.load_from_file(default_config_path(), error_message));
    EXPECT_TRUE(manager.validate(error_message));
    EXPECT_TRUE(error_message.empty());
}

TEST(ConfigManagerTest, DefaultConfigHasExpectedValues) {
    ConfigManager manager;

    std::string error_message;
    ASSERT_TRUE(manager.load_from_file(default_config_path(), error_message));

    const auto& config = manager.config();

    EXPECT_DOUBLE_EQ(config.target_temperature_c, 75.0);
    EXPECT_DOUBLE_EQ(config.stabilization_entry_temperature_c, 68.0);
    EXPECT_DOUBLE_EQ(config.running_min_temperature_c, 70.0);
    EXPECT_DOUBLE_EQ(config.running_max_temperature_c, 76.0);
    EXPECT_DOUBLE_EQ(config.preheat_heater_power_pct, 85.0);
    EXPECT_DOUBLE_EQ(config.control_heater_high_pct, 55.0);
    EXPECT_DOUBLE_EQ(config.control_heater_low_pct, 10.0);
    EXPECT_DOUBLE_EQ(config.control_cooler_pct, 30.0);
}

TEST(ConfigManagerTest, MissingFileFailsToLoad) {
    ConfigManager manager;

    std::string error_message;
    EXPECT_FALSE(manager.load_from_file("config/does_not_exist.json", error_message));
    EXPECT_FALSE(error_message.empty());
}

TEST(ConfigManagerTest, InvalidRunningRangeFailsValidation) {
    ConfigManager manager;
    SimulationConfig config {};

    config.running_min_temperature_c = 80.0;
    config.running_max_temperature_c = 70.0;

    manager.set_config_for_testing(config);

    std::string error_message;
    EXPECT_FALSE(manager.validate(error_message));
    EXPECT_FALSE(error_message.empty());
}

TEST(ConfigManagerTest, InvalidWarningThresholdFailsValidation) {
    ConfigManager manager;
    SimulationConfig config {};

    config.overtemperature_warning_c = 100.0;
    config.overtemperature_critical_c = 95.0;

    manager.set_config_for_testing(config);

    std::string error_message;
    EXPECT_FALSE(manager.validate(error_message));
    EXPECT_FALSE(error_message.empty());
}

TEST(ConfigManagerTest, CustomConfigLoadsExpectedValues) {
    const auto path = temp_test_config_path("test_custom_config.json");

    {
        std::ofstream out(path);
        out << R"({
  "target_temperature_c": 80.0,
  "control_heater_high_pct": 65.0,
  "total_ticks": 55
})";
    }

    ConfigManager manager;
    std::string error_message;

    ASSERT_TRUE(manager.load_from_file(path, error_message));

    const auto& config = manager.config();
    EXPECT_DOUBLE_EQ(config.target_temperature_c, 80.0);
    EXPECT_DOUBLE_EQ(config.control_heater_high_pct, 65.0);
    EXPECT_EQ(config.total_ticks, 55);

    std::remove(path.c_str());
}

TEST(ConfigManagerTest, MalformedJsonFailsToLoad) {
    const auto path = temp_test_config_path("test_bad_config.json");

    {
        std::ofstream out(path);
        out << R"({ "target_temperature_c": 75.0, )";
    }

    ConfigManager manager;
    std::string error_message;

    EXPECT_FALSE(manager.load_from_file(path, error_message));
    EXPECT_FALSE(error_message.empty());

    std::remove(path.c_str());
}