#include "config/ConfigManager.hpp"

#include <gtest/gtest.h>
#include <string>

using namespace thermal;

namespace {
std::string default_config_path() {
    return std::string(TEST_CONFIG_DIR) + "/default_config.json";
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