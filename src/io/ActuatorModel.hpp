#pragma once

#include "core/Types.hpp"

namespace thermal {

class ActuatorModel {
public:
    void set_commands(const ActuatorCommands& commands);
    [[nodiscard]] const ActuatorCommands& current_commands() const noexcept;

private:
    ActuatorCommands commands_ {};
};

} // namespace thermal