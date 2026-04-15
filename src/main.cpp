#include "app/Application.hpp"

#include <string>

int main(int argc, char* argv[]) {
    std::string config_path = "config/default_config.json";

    if (argc > 1) {
        config_path = argv[1];
    }

    thermal::Application app;
    return app.run(config_path);
}