#pragma once

#include "io/SensorSimulator.hpp"

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
    ScenarioRunner();

    void apply_events_for_tick(int tick_index, SensorSimulator& sensor_simulator) const noexcept;

    [[nodiscard]] const std::vector<ScenarioEvent>& events() const noexcept;

private:
    std::vector<ScenarioEvent> events_;
};

} // namespace thermal