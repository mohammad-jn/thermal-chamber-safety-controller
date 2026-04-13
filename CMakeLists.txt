cmake_minimum_required(VERSION 3.20)

project(ThermalChamberSafetyController
    VERSION 0.1.0
    DESCRIPTION "Safety-critical thermal chamber simulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
endif()

add_executable(thermal_chamber
    src/main.cpp
    src/app/Application.cpp
    src/core/StateMachine.cpp
    src/core/ChamberModel.cpp
    src/core/Controller.cpp
    src/io/SensorSimulator.cpp
    src/io/ActuatorModel.cpp
    src/diagnostics/ConsoleLogger.cpp
)

target_include_directories(thermal_chamber PRIVATE src)

option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()