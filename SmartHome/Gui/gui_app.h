#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QString>
#include <QThread>
#include <QTimer>

#include "Pages/Analytics/analytics_model.h"
#include "config_io.h"
#include "gui_state.h"

namespace smart_home::gui {

// UI-thread state; only immutable snapshots cross to the I/O worker.
class GuiApp : public QObject {
    Q_OBJECT
  public:
    explicit GuiApp(std::string path, QObject *parent = nullptr);
    ~GuiApp() override;
    const HomeSettings &settings() const {
        return m_state.pending;
    }
    bool loaded() const {
        return m_state.loaded;
    }
    bool busy() const {
        return m_state.busy;
    }
    bool editable() const {
        return m_state.loaded && !m_state.loading;
    }
    bool dirty() const;
    QString status() const;
    const GuiState &state() const {
        return m_state;
    }
    const AnalyticsModel &analytics() const {
        return m_analytics;
    }
    void setLight(Light light, bool on);
    void setAcOn(bool on);
    void stepAcMode(int direction);
    void setSpeaker(SpeakerControl control, int value);
    // Caller confirms discarding pending edits before requesting reload.
    void reload();
    void save();

  signals:
    void changed();
    void tick();

  private:
    std::string m_path;
    GuiState m_state;
    app::ConfigSnapshot m_accepted;
    AnalyticsModel m_analytics;
    QTimer m_timer;
    QElapsedTimer m_clock;
    void edit(const HomeSettings &settings);
    void sampleAnalytics();
    QString m_error;
    QThread m_thread;
    QObject *m_worker;
};
} // namespace smart_home::gui
