#include "core/StateMachine.hpp"

namespace thermal {

StateMachine::StateMachine()
    : state_(SystemState::Startup) {}

SystemState StateMachine::current_state() const noexcept {
    return state_;
}

TransitionResult StateMachine::transition_to(SystemState next_state) {
    if (!is_valid_transition(state_, next_state)) {
        return TransitionResult::InvalidTransition;
    }

    state_ = next_state;
    return TransitionResult::Success;
}

bool StateMachine::is_valid_transition(SystemState from, SystemState to) const noexcept {
    if (from == to) {
        return true;
    }

    switch (from) {
        case SystemState::Startup:
            return to == SystemState::SelfTest
                || to == SystemState::Fault
                || to == SystemState::EmergencyShutdown;

        case SystemState::SelfTest:
            return to == SystemState::Idle
                || to == SystemState::Fault
                || to == SystemState::EmergencyShutdown;

        case SystemState::Idle:
            return to == SystemState::Preheat
                || to == SystemState::Fault
                || to == SystemState::EmergencyShutdown;

        case SystemState::Preheat:
            return to == SystemState::Stabilizing
                || to == SystemState::Fault
                || to == SystemState::EmergencyShutdown;

        case SystemState::Stabilizing:
            return to == SystemState::Running
                || to == SystemState::Warning
                || to == SystemState::Fault
                || to == SystemState::EmergencyShutdown;

        case SystemState::Running:
            return to == SystemState::Warning
                || to == SystemState::Fault
                || to == SystemState::EmergencyShutdown
                || to == SystemState::Idle;

        case SystemState::Warning:
            return to == SystemState::Running
                || to == SystemState::Fault
                || to == SystemState::EmergencyShutdown
                || to == SystemState::Idle;

        case SystemState::Fault:
            return to == SystemState::Idle
                || to == SystemState::EmergencyShutdown;

        case SystemState::EmergencyShutdown:
            return false;

        default:
            return false;
    }
}

} // namespace thermal