#include "app/Application.hpp"

#include <string>

int main(int argc, char* argv[]) {
    std::string config_path = "config/default_config.json";
    std::string scenario_path = "scenarios/default_scenario.json";

    if (argc > 1) {
        config_path = argv[1];
    }

    if (argc > 2) {
        scenario_path = argv[2];
    }

    thermal::Application app;
    return app.run(config_path, scenario_path);
}