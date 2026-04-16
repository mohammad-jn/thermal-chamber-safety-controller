#include "scenarios/ScenarioRunner.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace thermal {

namespace {
using json = nlohmann::json;
}

bool ScenarioRunner::load_from_file(
    const std::string& file_path,
    std::string& error_message
) noexcept {
    std::ifstream input(file_path);
    if (!input.is_open()) {
        error_message = "Could not open scenario file: " + file_path;
        return false;
    }

    json j;
    try {
        input >> j;
    } catch (const std::exception& ex) {
        error_message = "Failed to parse scenario JSON: " + std::string(ex.what());
        return false;
    }

    if (!j.contains("events") || !j.at("events").is_array()) {
        error_message = "Scenario file must contain an 'events' array.";
        return false;
    }

    std::vector<ScenarioEvent> loaded_events;

    try {
        for (const auto& item : j.at("events")) {
            if (!item.contains("tick_index") || !item.contains("type")) {
                error_message = "Each scenario event must contain 'tick_index' and 'type'.";
                return false;
            }

            const int tick_index = item.at("tick_index").get<int>();
            const std::string type_str = item.at("type").get<std::string>();

            ScenarioEventType event_type {};
            if (!parse_event_type(type_str, event_type)) {
                error_message = "Unknown scenario event type: " + type_str;
                return false;
            }

            loaded_events.push_back(ScenarioEvent{tick_index, event_type});
        }
    } catch (const std::exception& ex) {
        error_message = "Invalid scenario event format: " + std::string(ex.what());
        return false;
    }

    events_ = std::move(loaded_events);
    error_message.clear();
    return true;
}

void ScenarioRunner::apply_events_for_tick(int tick_index, SensorSimulator& sensor_simulator) const noexcept {
    for (const auto& event : events_) {
        if (event.tick_index != tick_index) {
            continue;
        }

        switch (event.type) {
            case ScenarioEventType::OpenDoor:
                sensor_simulator.set_door_open(true);
                break;
            case ScenarioEventType::CloseDoor:
                sensor_simulator.set_door_open(false);
                break;
            case ScenarioEventType::SetSensorFreshFalse:
                sensor_simulator.set_sensor_fresh(false);
                break;
            case ScenarioEventType::SetSensorFreshTrue:
                sensor_simulator.set_sensor_fresh(true);
                break;
            case ScenarioEventType::SetPowerUnavailable:
                sensor_simulator.set_power_available(false);
                break;
            case ScenarioEventType::SetPowerAvailable:
                sensor_simulator.set_power_available(true);
                break;
        }
    }
}

const std::vector<ScenarioEvent>& ScenarioRunner::events() const noexcept {
    return events_;
}

bool ScenarioRunner::parse_event_type(
    const std::string& value,
    ScenarioEventType& out_type
) noexcept {
    if (value == "OpenDoor") {
        out_type = ScenarioEventType::OpenDoor;
        return true;
    }
    if (value == "CloseDoor") {
        out_type = ScenarioEventType::CloseDoor;
        return true;
    }
    if (value == "SetSensorFreshFalse") {
        out_type = ScenarioEventType::SetSensorFreshFalse;
        return true;
    }
    if (value == "SetSensorFreshTrue") {
        out_type = ScenarioEventType::SetSensorFreshTrue;
        return true;
    }
    if (value == "SetPowerUnavailable") {
        out_type = ScenarioEventType::SetPowerUnavailable;
        return true;
    }
    if (value == "SetPowerAvailable") {
        out_type = ScenarioEventType::SetPowerAvailable;
        return true;
    }

    return false;
}

} // namespace thermal