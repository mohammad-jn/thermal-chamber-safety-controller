#include "scenarios/ScenarioRunner.hpp"

namespace thermal {

ScenarioRunner::ScenarioRunner()
    : events_{
          {16, ScenarioEventType::OpenDoor},
          {20, ScenarioEventType::CloseDoor},
          {28, ScenarioEventType::SetSensorFreshFalse}
      } {}

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

} // namespace thermal