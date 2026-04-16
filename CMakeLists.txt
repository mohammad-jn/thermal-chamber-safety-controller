cmake_minimum_required(VERSION 3.20)

project(ThermalChamberSafetyController
    VERSION 0.1.0
    DESCRIPTION "Safety-critical thermal chamber simulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
endif()

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(thermal_chamber
    src/main.cpp
    src/app/Application.cpp
    src/config/ConfigManager.cpp
    src/core/StateMachine.cpp
    src/core/ChamberModel.cpp
    src/core/Controller.cpp
    src/core/SafetyManager.cpp
    src/core/FaultManager.cpp
    src/io/SensorSimulator.cpp
    src/io/ActuatorModel.cpp
    src/scenarios/ScenarioRunner.cpp
    src/diagnostics/ConsoleLogger.cpp
)

target_include_directories(thermal_chamber PRIVATE src)

target_link_libraries(thermal_chamber
    PRIVATE nlohmann_json::nlohmann_json
)

option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/scenarios DESTINATION ${CMAKE_BINARY_DIR})