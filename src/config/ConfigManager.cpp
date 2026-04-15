#include "config/ConfigManager.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace thermal {

namespace {
using json = nlohmann::json;
}

ConfigManager::ConfigManager() = default;

const SimulationConfig& ConfigManager::config() const noexcept {
    return config_;
}

bool ConfigManager::load_from_file(
    const std::string& file_path,
    std::string& error_message
) noexcept {
    std::ifstream input(file_path);
    if (!input.is_open()) {
        error_message = "Could not open config file: " + file_path;
        return false;
    }

    json j;
    try {
        input >> j;
    } catch (const std::exception& ex) {
        error_message = "Failed to parse JSON config: " + std::string(ex.what());
        return false;
    }

    try {
        if (j.contains("target_temperature_c")) {
            config_.target_temperature_c = j.at("target_temperature_c").get<double>();
        }
        if (j.contains("stabilization_entry_temperature_c")) {
            config_.stabilization_entry_temperature_c = j.at("stabilization_entry_temperature_c").get<double>();
        }
        if (j.contains("running_min_temperature_c")) {
            config_.running_min_temperature_c = j.at("running_min_temperature_c").get<double>();
        }
        if (j.contains("running_max_temperature_c")) {
            config_.running_max_temperature_c = j.at("running_max_temperature_c").get<double>();
        }
        if (j.contains("overtemperature_warning_c")) {
            config_.overtemperature_warning_c = j.at("overtemperature_warning_c").get<double>();
        }
        if (j.contains("overtemperature_critical_c")) {
            config_.overtemperature_critical_c = j.at("overtemperature_critical_c").get<double>();
        }
        if (j.contains("preheat_heater_power_pct")) {
            config_.preheat_heater_power_pct = j.at("preheat_heater_power_pct").get<double>();
        }
        if (j.contains("control_heater_high_pct")) {
            config_.control_heater_high_pct = j.at("control_heater_high_pct").get<double>();
        }
        if (j.contains("control_heater_low_pct")) {
            config_.control_heater_low_pct = j.at("control_heater_low_pct").get<double>();
        }
        if (j.contains("control_cooler_pct")) {
            config_.control_cooler_pct = j.at("control_cooler_pct").get<double>();
        }
        if (j.contains("control_band_c")) {
            config_.control_band_c = j.at("control_band_c").get<double>();
        }
        if (j.contains("ambient_temperature_c")) {
            config_.ambient_temperature_c = j.at("ambient_temperature_c").get<double>();
        }
        if (j.contains("dt_seconds")) {
            config_.dt_seconds = j.at("dt_seconds").get<double>();
        }
        if (j.contains("total_ticks")) {
            config_.total_ticks = j.at("total_ticks").get<int>();
        }
    } catch (const std::exception& ex) {
        error_message = "Invalid config field type: " + std::string(ex.what());
        return false;
    }

    if (!validate(error_message)) {
        return false;
    }

    error_message.clear();
    return true;
}

bool ConfigManager::validate(std::string& error_message) const noexcept {
    if (config_.target_temperature_c <= 0.0) {
        error_message = "Target temperature must be positive.";
        return false;
    }

    if (config_.stabilization_entry_temperature_c <= 0.0) {
        error_message = "Stabilization entry temperature must be positive.";
        return false;
    }

    if (config_.running_min_temperature_c > config_.running_max_temperature_c) {
        error_message = "Running temperature range is invalid.";
        return false;
    }

    if (config_.overtemperature_warning_c >= config_.overtemperature_critical_c) {
        error_message = "Warning temperature must be lower than critical temperature.";
        return false;
    }

    if (config_.preheat_heater_power_pct < 0.0 || config_.preheat_heater_power_pct > 100.0) {
        error_message = "Preheat heater power must be between 0 and 100.";
        return false;
    }

    if (config_.control_heater_high_pct < 0.0 || config_.control_heater_high_pct > 100.0) {
        error_message = "High heater power must be between 0 and 100.";
        return false;
    }

    if (config_.control_heater_low_pct < 0.0 || config_.control_heater_low_pct > 100.0) {
        error_message = "Low heater power must be between 0 and 100.";
        return false;
    }

    if (config_.control_cooler_pct < 0.0 || config_.control_cooler_pct > 100.0) {
        error_message = "Cooler power must be between 0 and 100.";
        return false;
    }

    if (config_.control_band_c <= 0.0) {
        error_message = "Control band must be positive.";
        return false;
    }

    if (config_.dt_seconds <= 0.0) {
        error_message = "Time step must be positive.";
        return false;
    }

    if (config_.total_ticks <= 0) {
        error_message = "Total ticks must be positive.";
        return false;
    }

    error_message.clear();
    return true;
}

void ConfigManager::set_config_for_testing(const SimulationConfig& config) noexcept {
    config_ = config;
}

} // namespace thermal