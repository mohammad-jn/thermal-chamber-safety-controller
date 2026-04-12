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

} // namespace thermal