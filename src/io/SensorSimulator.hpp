#pragma once

#include "core/Types.hpp"

namespace thermal {

class SensorSimulator {
public:
    SensorSimulator() = default;

    [[nodiscard]] SensorReadings read(double chamber_temperature_c) const noexcept;

    void set_door_open(bool value) noexcept;
    void set_power_available(bool value) noexcept;
    void set_sensor_fresh(bool value) noexcept;
    void set_airflow_ok(bool value) noexcept;
    void set_ambient_temperature_c(double value) noexcept;

private:
    double ambient_temperature_c_ {22.0};
    bool door_open_ {false};
    bool airflow_ok_ {true};
    bool power_available_ {true};
    bool sensor_fresh_ {true};
};

} // namespace thermal