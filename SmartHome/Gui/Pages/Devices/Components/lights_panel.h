#pragma once
#include <QWidget>
#include <memory>

#include "gui_state.h"
namespace Ui {
class LightsPanel;
}
class LightsPanel : public QWidget {
    Q_OBJECT
  public:
    explicit LightsPanel(QWidget *parent = nullptr);
    ~LightsPanel() override;
    void render(const LightSettings &snapshot, bool available);
  signals:
    void lightRequested(smart_home::gui::Light light, bool on);

  private:
    std::unique_ptr<Ui::LightsPanel> m_ui;
};
