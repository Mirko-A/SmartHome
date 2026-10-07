#pragma once

#include <QWidget>
#include <memory>

namespace Ui {

class DevicesPage;

} // namespace Ui

namespace smart_home::gui {

class App;

} // namespace smart_home::gui

class DevicesPage : public QWidget {
    Q_OBJECT

  public:
    explicit DevicesPage(smart_home::gui::App &app, QWidget *parent = nullptr);
    ~DevicesPage() override;

  private:
    void render();

  private:
    smart_home::gui::App &m_app;
    std::unique_ptr<Ui::DevicesPage> m_ui;
};
