#include "gui.h"

#include <qapplication.h>

#include <filesystem>
#include <iostream>

#include "main_window.h"

namespace smart_home::gui {

int main(int argc, char *argv[]) {
    std::filesystem::path configDirectory = "../Resources";
    if (argc == 3 && std::string(argv[1]) == "--config-dir" && argv[2][0] != '\0') {
        configDirectory = argv[2];
    } else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " gui [--config-dir DIRECTORY]\n";
        return 1;
    }
    // Qt receives only argv[0]; application options have already been consumed.
    int qtArgc = 1;
    char *qtArgv[] = {argv[0], nullptr};
    QApplication app(qtArgc, qtArgv);
    MainWindow window((configDirectory / "home_cfg.json").string());
    window.setWindowTitle("Smart Home");

    window.show();
    return app.exec();
}

} // namespace smart_home::gui
