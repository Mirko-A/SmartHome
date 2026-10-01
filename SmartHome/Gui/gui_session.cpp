#include "gui_session.h"

#include <QMetaObject>

namespace smart_home::gui {
GuiSession::GuiSession(std::string path, QObject *parent)
    : QObject(parent), m_path(std::move(path)), m_worker(new QObject) {
    m_worker->moveToThread(&m_thread);
    connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
    m_thread.start();
}

GuiSession::~GuiSession() {
    // Finish any in-flight file operation before destroying its completion receiver.
    m_thread.quit();
    m_thread.wait();
}

bool GuiSession::dirty() const {
    return m_loaded && m_pending.serializeJson() != m_saved.serializeJson();
}

QString GuiSession::status() const {
    if (m_busy)
        return m_loading ? "Loading settings…" : "Saving settings…";
    if (!m_error.isEmpty())
        return m_error;
    if (!m_loaded)
        return "No valid configuration loaded. Use Reload to retry.";
    return dirty() ? "Unsaved settings — device application is unavailable."
                   : "No unsaved settings — device application is unavailable.";
}

void GuiSession::edit(const HomeSettings &settings) {
    if (!editable())
        return;
    m_pending = settings;
    // Keep an outstanding I/O error visible even when the user continues editing.
    emit changed();
}

void GuiSession::reload() {
    if (m_busy)
        return;
    m_busy = m_loading = true;
    m_error.clear();
    emit changed();
    QMetaObject::invokeMethod(
        m_worker,
        [this, path = m_path] {
            auto result = app::loadConfig(path);
            HomeSettings candidate;
            if (result) {
                if (auto valid = candidate.deserializeJson(result->document); !valid) {
                    result = std::unexpected(
                        app::ConfigError{app::ConfigErrorKind::VALIDATION,
                                         "Invalid settings (" + path + "): " + valid.error()});
                }
            }
            QMetaObject::invokeMethod(
                this,
                [this, result = std::move(result), candidate] {
                    m_busy = m_loading = false;
                    if (result) {
                        m_accepted = std::move(*result);
                        m_pending = m_saved = candidate;
                        m_loaded = true;
                    } else {
                        m_error = QString::fromStdString(result.error().message);
                    }
                    emit changed();
                },
                Qt::QueuedConnection);
        },
        Qt::QueuedConnection);
}

void GuiSession::save() {
    if (m_busy || !m_loaded || !dirty())
        return;
    m_busy = true;
    m_error.clear();
    auto snapshot = m_pending;
    auto document = m_accepted.document;
    document.merge_patch(snapshot.serializeJson());
    emit changed();
    QMetaObject::invokeMethod(
        m_worker,
        [this, path = m_path, document = std::move(document), version = m_accepted.version,
         snapshot] {
            auto result = app::saveConfig(path, document, version);
            QMetaObject::invokeMethod(
                this,
                [this, result = std::move(result), snapshot] {
                    m_busy = false;
                    if (result) {
                        m_accepted = std::move(*result);
                        // Newer UI edits remain pending; never replace them with this saved
                        // snapshot.
                        m_saved = snapshot;
                    } else {
                        m_error = QString::fromStdString(result.error().message);
                    }
                    emit changed();
                },
                Qt::QueuedConnection);
        },
        Qt::QueuedConnection);
}
} // namespace smart_home::gui
