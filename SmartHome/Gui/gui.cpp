#include "gui.h"

#include <qapplication.h>

#include "main_window.h"

namespace smart_home::gui {

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("Smart Home");

    window.show();
    return app.exec();
}

} // namespace smart_home::gui
