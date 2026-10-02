#include <iostream>
#include <string>

#ifdef SMARTHOME_BUILD_GUI
#include "gui.h"
#endif
#include "tui.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <mode> [--config-dir DIRECTORY]" << std::endl;
        std::cerr << "Modes: gui, tui" << std::endl;
        return 1;
    }

    // Copy the mode argument, then remove it from argv.
    std::string mode = argv[1];
    for (int i = 2; i < argc; ++i) {
        argv[i - 1] = argv[i];
    }
    argc -= 1;

    if (mode == "gui") {
#ifdef SMARTHOME_BUILD_GUI
        return smart_home::gui::main(argc, argv);
#else
        std::cerr << "GUI support is disabled. Configure with -DSMARTHOME_BUILD_GUI=ON.\n";
        return 1;
#endif
    } else if (mode == "tui") {
        return smart_home::tui::main(argc, argv);
    } else {
        std::cerr << "Unknown mode: " << mode << std::endl;
        return 1;
    }
}
