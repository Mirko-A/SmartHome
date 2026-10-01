#include "gui_app.h"

#include <QMetaObject>

namespace smart_home::gui {
GuiApp::GuiApp(std::string path, QObject *parent)
    : QObject(parent), m_path(std::move(path)), m_worker(new QObject) {
    m_worker->moveToThread(&m_thread);
    connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
    m_thread.start();
    connect(&m_timer, &QTimer::timeout, this, [this] { emit tick(); });
    m_timer.start(100);
}

GuiApp::~GuiApp() {
    // Finish any in-flight file operation before destroying its completion receiver.
    m_timer.stop();
    m_thread.quit();
    m_thread.wait();
}

bool GuiApp::dirty() const {
    return m_state.loaded && m_state.pending.serializeJson() != m_state.saved.serializeJson();
}

QString GuiApp::status() const {
    if (m_state.busy)
        return m_state.loading ? "Loading settings…" : "Saving settings…";
    if (!m_error.isEmpty())
        return m_error;
    if (!m_state.loaded)
        return "No valid configuration loaded. Use Reload to retry.";
    return dirty() ? "Unsaved settings — device application is unavailable."
                   : "No unsaved settings — device application is unavailable.";
}

void GuiApp::edit(const HomeSettings &settings) {
    if (!editable())
        return;
    m_state.pending = settings;

    // Keep an outstanding I/O error visible even when the user continues editing.
    emit changed();
}

void GuiApp::reload() {
    if (m_state.busy)
        return;
    m_state.busy = m_state.loading = true;
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
                    m_state.busy = m_state.loading = false;
                    if (result) {
                        m_accepted = std::move(*result);
                        m_state.pending = m_state.saved = candidate;
                        m_state.loaded = true;

                    } else {
                        m_error = QString::fromStdString(result.error().message);
                    }
                    emit changed();
                },
                Qt::QueuedConnection);
        },
        Qt::QueuedConnection);
}

void GuiApp::save() {
    if (m_state.busy || !m_state.loaded || !dirty())
        return;
    m_state.busy = true;
    m_error.clear();
    auto snapshot = m_state.pending;
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
                    m_state.busy = false;
                    if (result) {
                        m_accepted = std::move(*result);
                        // Newer UI edits remain pending; never replace them with this saved
                        // snapshot.
                        m_state.saved = snapshot;
                    } else {
                        m_error = QString::fromStdString(result.error().message);
                    }
                    emit changed();
                },
                Qt::QueuedConnection);
        },
        Qt::QueuedConnection);
}

void GuiApp::setLight(Light light, bool on) {
    auto settings = m_state.pending;
    auto lights = settings.lights();
    switch (light) {
    case Light::LIVING_ROOM:
        lights.livingRoomLightOn = on;
        break;
    case Light::BEDROOM:
        lights.bedroomLightOn = on;
        break;
    case Light::KITCHEN:
        lights.kitchenLightOn = on;
        break;
    }
    settings.setLights(lights.livingRoomLightOn, lights.bedroomLightOn, lights.kitchenLightOn);
    edit(settings);
}
void GuiApp::setAcOn(bool on) {
    auto settings = m_state.pending;
    settings.setAc(on, settings.ac().mode);
    edit(settings);
}
void GuiApp::stepAcMode(int direction) {
    auto settings = m_state.pending;
    const int mode = static_cast<int>(settings.ac().mode) + (direction > 0 ? 1 : -1);
    if (direction == 0 || mode < static_cast<int>(Ac::Mode::NORMAL) ||
        mode > static_cast<int>(Ac::Mode::TURBO))
        return;
    settings.setAc(settings.ac().on, static_cast<Ac::Mode>(mode));
    edit(settings);
}
void GuiApp::setSpeaker(SpeakerControl control, int value) {
    if (!editable())
        return;
    auto settings = m_state.pending;
    const auto speakers = settings.speakers();
    auto result = settings.setSpeakers(control == SpeakerControl::VOLUME ? value : speakers.volume,
                                       control == SpeakerControl::BASS ? value : speakers.bass,
                                       control == SpeakerControl::PITCH ? value : speakers.pitch);
    if (!result) {
        m_error = QString::fromStdString(result.error());
        emit changed();
        return;
    }
    edit(settings);
}
} // namespace smart_home::gui
