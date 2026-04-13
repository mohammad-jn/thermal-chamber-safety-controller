#include "core/StateMachine.hpp"
#include "core/Types.hpp"

#include <gtest/gtest.h>

using namespace thermal;

TEST(StateMachineTest, InitialStateIsStartup) {
    StateMachine sm;
    EXPECT_EQ(sm.current_state(), SystemState::Startup);
}

TEST(StateMachineTest, AllowsValidStartupSequence) {
    StateMachine sm;

    EXPECT_EQ(sm.transition_to(SystemState::SelfTest), TransitionResult::Success);
    EXPECT_EQ(sm.current_state(), SystemState::SelfTest);

    EXPECT_EQ(sm.transition_to(SystemState::Idle), TransitionResult::Success);
    EXPECT_EQ(sm.current_state(), SystemState::Idle);

    EXPECT_EQ(sm.transition_to(SystemState::Preheat), TransitionResult::Success);
    EXPECT_EQ(sm.current_state(), SystemState::Preheat);

    EXPECT_EQ(sm.transition_to(SystemState::Stabilizing), TransitionResult::Success);
    EXPECT_EQ(sm.current_state(), SystemState::Stabilizing);

    EXPECT_EQ(sm.transition_to(SystemState::Running), TransitionResult::Success);
    EXPECT_EQ(sm.current_state(), SystemState::Running);
}

TEST(StateMachineTest, RejectsInvalidTransitionFromStartupToRunning) {
    StateMachine sm;

    EXPECT_EQ(sm.transition_to(SystemState::Running), TransitionResult::InvalidTransition);
    EXPECT_EQ(sm.current_state(), SystemState::Startup);
}

TEST(StateMachineTest, RejectsInvalidTransitionFromIdleToRunning) {
    StateMachine sm;

    EXPECT_EQ(sm.transition_to(SystemState::SelfTest), TransitionResult::Success);
    EXPECT_EQ(sm.transition_to(SystemState::Idle), TransitionResult::Success);

    EXPECT_EQ(sm.transition_to(SystemState::Running), TransitionResult::InvalidTransition);
    EXPECT_EQ(sm.current_state(), SystemState::Idle);
}

TEST(StateMachineTest, AllowsFaultToIdleRecovery) {
    StateMachine sm;

    EXPECT_EQ(sm.transition_to(SystemState::SelfTest), TransitionResult::Success);
    EXPECT_EQ(sm.transition_to(SystemState::Fault), TransitionResult::Success);
    EXPECT_EQ(sm.current_state(), SystemState::Fault);

    EXPECT_EQ(sm.transition_to(SystemState::Idle), TransitionResult::Success);
    EXPECT_EQ(sm.current_state(), SystemState::Idle);
}

TEST(StateMachineTest, EmergencyShutdownIsTerminal) {
    StateMachine sm;

    EXPECT_EQ(sm.transition_to(SystemState::SelfTest), TransitionResult::Success);
    EXPECT_EQ(sm.transition_to(SystemState::EmergencyShutdown), TransitionResult::Success);
    EXPECT_EQ(sm.current_state(), SystemState::EmergencyShutdown);

    EXPECT_EQ(sm.transition_to(SystemState::Idle), TransitionResult::InvalidTransition);
    EXPECT_EQ(sm.current_state(), SystemState::EmergencyShutdown);
}