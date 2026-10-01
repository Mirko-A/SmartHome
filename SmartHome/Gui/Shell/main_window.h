#pragma once
#include <QMainWindow>
#include <memory>
namespace Ui {
class MainWindow;
}
namespace smart_home::gui {
class GuiApp;
}
class QAction;
class QLabel;
class QCloseEvent;
class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    explicit MainWindow(smart_home::gui::GuiApp &app, QWidget *parent = nullptr);
    ~MainWindow() override;

  private:
    void selectPage(int index);
    void refreshSession();
    void reloadSettings();
    void closeEvent(QCloseEvent *event) override;
    smart_home::gui::GuiApp &m_app;
    std::unique_ptr<Ui::MainWindow> m_ui;
    QAction *m_saveAction;
    QAction *m_reloadAction;
    QLabel *m_configStatus;
};
