#pragma once

#include "io/SensorSimulator.hpp"

#include <string>
#include <vector>

namespace thermal {

enum class ScenarioEventType {
    OpenDoor,
    CloseDoor,
    SetSensorFreshFalse,
    SetSensorFreshTrue,
    SetPowerUnavailable,
    SetPowerAvailable
};

struct ScenarioEvent {
    int tick_index;
    ScenarioEventType type;
};

class ScenarioRunner {
public:
    ScenarioRunner() = default;

    [[nodiscard]] bool load_from_file(
        const std::string& file_path,
        std::string& error_message
    ) noexcept;

    void apply_events_for_tick(int tick_index, SensorSimulator& sensor_simulator) const noexcept;

    [[nodiscard]] const std::vector<ScenarioEvent>& events() const noexcept;

private:
    [[nodiscard]] static bool parse_event_type(
        const std::string& value,
        ScenarioEventType& out_type
    ) noexcept;

    std::vector<ScenarioEvent> events_;
};

} // namespace thermal