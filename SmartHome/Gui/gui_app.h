#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QString>
#include <QThread>
#include <QTimer>

#include "Pages/Analytics/analytics_model.h"
#include "app_session.h"
#include "gui_state.h"

namespace smart_home::gui {

// Devices and editable state stay on the UI thread; workers only perform file I/O.
class App : public QObject {
    Q_OBJECT

  public:
    explicit App(std::string path, QObject *parent = nullptr);
    ~App() override;

    bool loaded() const {
        return m_session.loaded();
    }
    bool busy() const {
        return m_busy;
    }
    bool editable() const {
        return loaded() && !m_loading && !m_closing;
    }
    bool hasUnsavedChanges() const {
        return m_session.hasUnsavedChanges();
    }
    QString status() const;
    const app::AppState &state() const {
        return m_session.state();
    }
    const AnalyticsModel &analytics() const {
        return m_analytics;
    }
    void setLight(Light light, bool on);
    void setAcOn(bool on);
    void stepAcMode(int direction);
    void setSpeaker(SpeakerControl control, int value);
    void reload();
    void save();
    void saveAndClose();

  signals:
    void changed();
    void tick();
    void shutdownReady();

  private:
    void edited();
    void startSave(bool closing);
    void sampleAnalytics();

    app::AppSession m_session;
    bool m_busy = false;
    bool m_loading = false;
    bool m_closing = false;
    AnalyticsModel m_analytics;
    QTimer m_timer;
    QElapsedTimer m_clock;
    QString m_error;
    QThread m_thread;
    QObject *m_worker;
};

} // namespace smart_home::gui
