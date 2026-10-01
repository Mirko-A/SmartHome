#pragma once
#include <QWidget>
#include <memory>

#include "gui_state.h"
namespace Ui {
class SpeakersPanel;
}
class SpeakersPanel : public QWidget {
    Q_OBJECT
  public:
    explicit SpeakersPanel(QWidget *parent = nullptr);
    ~SpeakersPanel() override;
    void render(const SpeakerSettings &snapshot, bool available);
  signals:
    void valueRequested(smart_home::gui::SpeakerControl control, int value);

  private:
    std::unique_ptr<Ui::SpeakersPanel> m_ui;
};
