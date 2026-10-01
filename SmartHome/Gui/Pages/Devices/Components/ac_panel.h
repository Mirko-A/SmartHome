#pragma once
#include <QWidget>
#include <memory>

#include "gui_state.h"
namespace Ui {
class AcPanel;
}
class AcPanel : public QWidget {
    Q_OBJECT
  public:
    explicit AcPanel(QWidget *parent = nullptr);
    ~AcPanel() override;
    void render(const AcSettings &snapshot, bool available);
  signals:
    void onRequested(bool on);
    void modeStepRequested(int direction);

  private:
    std::unique_ptr<Ui::AcPanel> m_ui;
};
