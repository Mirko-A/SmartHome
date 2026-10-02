#pragma once

#include <QObject>
#include <QString>
#include <QThread>

#include "config_io.h"
#include "home_settings.h"

namespace smart_home::gui {

// UI-thread state; only immutable snapshots cross to the I/O worker.
class GuiSession : public QObject {
    Q_OBJECT
  public:
    explicit GuiSession(std::string path, QObject *parent = nullptr);
    ~GuiSession() override;
    const HomeSettings &settings() const {
        return m_pending;
    }
    bool loaded() const {
        return m_loaded;
    }
    bool busy() const {
        return m_busy;
    }
    bool editable() const {
        return m_loaded && !m_loading;
    }
    bool dirty() const;
    QString status() const;
    void edit(const HomeSettings &settings);
    // Caller confirms discarding pending edits before requesting reload.
    void reload();
    void save();

  signals:
    void changed();

  private:
    std::string m_path;
    HomeSettings m_pending;
    HomeSettings m_saved;
    app::ConfigSnapshot m_accepted;
    bool m_loaded = false;
    bool m_busy = false;
    bool m_loading = false;
    QString m_error;
    QThread m_thread;
    QObject *m_worker;
};
} // namespace smart_home::gui
