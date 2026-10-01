#pragma once
#include <QWidget>
#include <memory>
namespace Ui {
class DevicesPage;
}
namespace smart_home::gui {
class GuiApp;
}
class DevicesPage : public QWidget {
    Q_OBJECT
  public:
    explicit DevicesPage(smart_home::gui::GuiApp &app, QWidget *parent = nullptr);
    ~DevicesPage() override;

  private:
    void render();
    smart_home::gui::GuiApp &m_app;
    std::unique_ptr<Ui::DevicesPage> m_ui;
};
