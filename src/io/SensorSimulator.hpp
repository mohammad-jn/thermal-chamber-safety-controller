#pragma once

#include "core/Types.hpp"

namespace thermal {

class SensorSimulator {
public:
    SensorSimulator() = default;

    [[nodiscard]] SensorReadings read(double chamber_temperature_c) const noexcept;

private:
    double ambient_temperature_c_ {22.0};
    bool door_open_ {false};
    bool airflow_ok_ {true};
    bool power_available_ {true};
    bool sensor_fresh_ {true};
};

} // namespace thermal