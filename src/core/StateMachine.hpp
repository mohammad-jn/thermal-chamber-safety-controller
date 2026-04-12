#pragma once

#include "core/Types.hpp"

namespace thermal {

class StateMachine {
public:
    StateMachine();

    [[nodiscard]] SystemState current_state() const noexcept;
    [[nodiscard]] TransitionResult transition_to(SystemState next_state);

private:
    [[nodiscard]] bool is_valid_transition(SystemState from, SystemState to) const noexcept;

    SystemState state_;
};

} // namespace thermal