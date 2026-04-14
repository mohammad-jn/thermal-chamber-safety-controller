#include "core/FaultManager.hpp"

namespace thermal {

FaultList FaultManager::detect_faults(
    const SensorReadings& sensors,
    const ActuatorCommands& applied_commands
) const {
    FaultList faults;

    if (!sensors.power_available) {
        faults.push_back(FaultRecord{
            FaultType::PowerLoss,
            FaultSeverity::Critical,
            "Power is unavailable."
        });
    }

    if (!sensors.sensor_fresh) {
        faults.push_back(FaultRecord{
            FaultType::SensorStale,
            FaultSeverity::Critical,
            "Sensor data is stale."
        });
    }

    if (sensors.door_open && applied_commands.heater_power_pct > 0.0) {
        faults.push_back(FaultRecord{
            FaultType::DoorOpenDuringActiveHeating,
            FaultSeverity::Critical,
            "Door is open while active heating is requested."
        });
    }

    if (sensors.chamber_temperature_c >= 95.0) {
        faults.push_back(FaultRecord{
            FaultType::OverTemperatureCritical,
            FaultSeverity::Critical,
            "Chamber temperature exceeded critical threshold."
        });
    } else if (sensors.chamber_temperature_c >= 85.0) {
        faults.push_back(FaultRecord{
            FaultType::OverTemperatureWarning,
            FaultSeverity::Warning,
            "Chamber temperature exceeded warning threshold."
        });
    }

    return faults;
}

bool FaultManager::has_critical_fault(const FaultList& faults) const noexcept {
    for (const auto& fault : faults) {
        if (fault.severity == FaultSeverity::Critical) {
            return true;
        }
    }
    return false;
}

bool FaultManager::has_warning_fault(const FaultList& faults) const noexcept {
    for (const auto& fault : faults) {
        if (fault.severity == FaultSeverity::Warning) {
            return true;
        }
    }
    return false;
}

} // namespace thermal