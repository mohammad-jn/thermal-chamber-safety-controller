#include "config/ConfigManager.hpp"

namespace thermal {

ConfigManager::ConfigManager() = default;

const SimulationConfig& ConfigManager::config() const noexcept {
    return config_;
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

} // namespace thermal