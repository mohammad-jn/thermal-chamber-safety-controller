#include "io/SensorSimulator.hpp"

namespace thermal {

SensorReadings SensorSimulator::read(double chamber_temperature_c) const noexcept {
    SensorReadings readings;
    readings.chamber_temperature_c = chamber_temperature_c;
    readings.ambient_temperature_c = ambient_temperature_c_;
    readings.door_open = door_open_;
    readings.airflow_ok = airflow_ok_;
    readings.power_available = power_available_;
    readings.sensor_fresh = sensor_fresh_;
    return readings;
}

void SensorSimulator::set_door_open(bool value) noexcept {
    door_open_ = value;
}

void SensorSimulator::set_power_available(bool value) noexcept {
    power_available_ = value;
}

void SensorSimulator::set_sensor_fresh(bool value) noexcept {
    sensor_fresh_ = value;
}

void SensorSimulator::set_airflow_ok(bool value) noexcept {
    airflow_ok_ = value;
}

void SensorSimulator::set_ambient_temperature_c(double value) noexcept {
    ambient_temperature_c_ = value;
}

} // namespace thermal