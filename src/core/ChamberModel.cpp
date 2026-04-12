#include "core/ChamberModel.hpp"

#include <algorithm>

namespace thermal {

ChamberModel::ChamberModel()
    : chamber_temperature_c_(25.0) {}

void ChamberModel::update(double dt_seconds,
                          double heater_power_pct,
                          double cooler_power_pct,
                          bool fan_on,
                          double ambient_temperature_c) {
    const double heater_gain = 0.10 * heater_power_pct;
    const double cooler_gain = 0.12 * cooler_power_pct;
    const double leakage = 0.03 * (chamber_temperature_c_ - ambient_temperature_c);
    const double fan_bonus = fan_on ? 0.05 : 0.0;

    const double delta =
        (heater_gain - cooler_gain - leakage - fan_bonus * (chamber_temperature_c_ - ambient_temperature_c))
        * dt_seconds;

    chamber_temperature_c_ += delta;
    chamber_temperature_c_ = std::max(-40.0, std::min(200.0, chamber_temperature_c_));
}

double ChamberModel::chamber_temperature_c() const noexcept {
    return chamber_temperature_c_;
}

} // namespace thermal