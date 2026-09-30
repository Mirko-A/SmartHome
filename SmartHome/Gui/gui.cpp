#include "gui.h"

#include <iostream>

namespace smart_home::gui {

int main(int argc, char *argv[]) {
#if 0
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("Smart Home");

    window.show();
    return app.exec();
#else
    (void)argc;
    (void)argv;
    std::cout << "GUI mode stub" << std::endl;
    return 0;
#endif
}

} // namespace smart_home::gui
