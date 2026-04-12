#pragma once

namespace thermal {

class ChamberModel {
public:
    ChamberModel();

    void update(double dt_seconds,
                double heater_power_pct,
                double cooler_power_pct,
                bool fan_on,
                double ambient_temperature_c);

    [[nodiscard]] double chamber_temperature_c() const noexcept;

private:
    double chamber_temperature_c_;
};

} // namespace thermal