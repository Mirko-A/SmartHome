#pragma once

#include <QMainWindow>
#include <memory>

namespace Ui {

class MainWindow;

} // namespace Ui

namespace smart_home::gui {

class App;

} // namespace smart_home::gui

class QAction;
class QLabel;
class QCloseEvent;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(smart_home::gui::App &app, QWidget *parent = nullptr);
    ~MainWindow() override;

  private:
    void selectPage(int index);
    void refreshSession();
    void reloadSettings();
    void closeEvent(QCloseEvent *event) override;

  private:
    smart_home::gui::App &m_app;
    std::unique_ptr<Ui::MainWindow> m_ui;

    QAction *m_saveAction;
    QAction *m_reloadAction;
    QLabel *m_configStatus;
};
