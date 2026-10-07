#include "tui.h"

#include <filesystem>
#include <iostream>
#include <utility>

#include "config_io.h"
#include "home_ctrl.h"
#include "tui_app.h"

namespace smart_home::tui {

int main(int argc, char *argv[]) {
    std::filesystem::path configDirectory = "../Resources";
    if (argc == 3 && std::string(argv[1]) == "--config-dir" && argv[2][0] != '\0') {
        configDirectory = argv[2];
    } else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " tui [--config-dir DIRECTORY]" << std::endl;
        return 1;
    }
    const auto pinConfigPath = (configDirectory / "pin_cfg.json").string();
    const auto homeConfigPath = (configDirectory / "home_cfg.json").string();

    auto pinConfig = app::loadConfig(pinConfigPath);
    if (!pinConfig) {
        std::cerr << "Error: " << pinConfig.error().message << std::endl;
        return -1;
    }
    auto homeConfig = app::loadConfig(homeConfigPath);
    if (!homeConfig) {
        std::cerr << "Error: " << homeConfig.error().message << std::endl;
        return -1;
    }
    auto homeResult = HomeControl::create(pinConfig->document, homeConfig->document);
    if (!homeResult) {
        std::cerr << "Error initializing home: " << homeResult.error() << std::endl;
        return -1;
    }
    HomeControl home = std::move(*homeResult);
    App app(home, homeConfigPath, std::move(*homeConfig));
    app.run();
    return 0;
}

} // namespace smart_home::tui
