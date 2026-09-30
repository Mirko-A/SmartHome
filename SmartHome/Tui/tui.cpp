#include "tui.h"

#include <iostream>
#include <utility>

#include "config_io.h"
#include "home_ctrl.h"
#include "tui_app.h"

namespace smart_home::tui {

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    auto pinConfig = loadConfig(PIN_CFG_FILE_PATH);
    if (!pinConfig) {
        std::cerr << "Error: " << pinConfig.error() << std::endl;
        return -1;
    }
    auto homeConfig = loadConfig(HOME_CFG_FILE_PATH, HOME_INI_FILE_PATH);
    if (!homeConfig) {
        std::cerr << "Error: " << homeConfig.error() << std::endl;
        return -1;
    }
    auto homeResult = HomeControl::create(*pinConfig);
    if (!homeResult) {
        std::cerr << "Error initializing pins: " << homeResult.error() << std::endl;
        return -1;
    }
    HomeControl home = std::move(*homeResult);
    if (auto result = home.deserializeJson(*homeConfig); !result) {
        std::cerr << "Error loading home settings: " << result.error() << std::endl;
        return -1;
    }

    TuiApp app(home);
    app.run();
    return 0;
}

} // namespace smart_home::tui
