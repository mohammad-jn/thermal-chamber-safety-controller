include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(thermal_chamber_tests
    test_state_machine.cpp
    test_controller.cpp
    test_safety_manager.cpp
    ../src/core/StateMachine.cpp
    ../src/core/Controller.cpp
    ../src/core/SafetyManager.cpp
)

target_include_directories(thermal_chamber_tests PRIVATE ../src)

target_link_libraries(thermal_chamber_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(thermal_chamber_tests)