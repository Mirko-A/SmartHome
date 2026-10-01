#pragma once
#include <QWidget>
#include <memory>

#include "gui_state.h"
namespace Ui {
class SensorsPanel;
}
class SensorsPanel : public QWidget {
    Q_OBJECT
  public:
    explicit SensorsPanel(QWidget *parent = nullptr);
    ~SensorsPanel() override;
    void render(const SensorReadings &snapshot, bool available);
  signals:

  private:
    std::unique_ptr<Ui::SensorsPanel> m_ui;
};
