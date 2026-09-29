#if 0
#include <QApplication>

#include "mainwindow.h"
#endif

#include <iostream>
#include <string>

#include "gui.h"
#include "tui.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <mode>" << std::endl;
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
        return gui_main(argc, argv);
    } else if (mode == "tui") {
        return tui_main(argc, argv);
    } else {
        std::cerr << "Unknown mode: " << mode << std::endl;
        return 1;
    }
}
