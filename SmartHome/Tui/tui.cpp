#include "tui.h"

#include <filesystem>
#include <iostream>
#include <utility>

#include "app_session.h"
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
    app::AppSession session((configDirectory / "home_cfg.json").string());
    if (auto result = session.reload(); !result) {
        std::cerr << "Error initializing home: " << result.error() << std::endl;
        return 1;
    }
    App app(session);
    app.run();
    return 0;
}

} // namespace smart_home::tui
