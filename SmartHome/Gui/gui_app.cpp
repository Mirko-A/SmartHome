#include "gui_app.h"

#include <QMetaObject>

namespace smart_home::gui {

App::App(std::string path, QObject *parent)
    : QObject(parent), m_session(std::move(path)), m_worker(new QObject) {
    m_worker->moveToThread(&m_thread);
    connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
    m_thread.start();
    m_clock.start();
    connect(&m_timer, &QTimer::timeout, this, [this] {
        m_session.update();
        sampleAnalytics();
        emit changed();
        emit tick();
    });
    m_timer.start(500);
}

App::~App() {
    m_timer.stop();
    m_thread.quit();
    m_thread.wait();
}

QString App::status() const {
    if (m_busy) {
        return m_loading ? "Loading settings…" : "Saving settings…";
    }
    QString status = m_error;
    if (!status.isEmpty()) {
        status += " ";
    }
    if (loaded()) {
        status += hasUnsavedChanges() ? "Unsaved edits. " : "No unsaved edits. ";
    }
    return status + QString::fromStdString(m_session.deviceStatus());
}

void App::edited() {
    m_session.edited();
    sampleAnalytics();
    emit changed();
}

void App::reload() {
    if (m_busy || m_closing) {
        return;
    }
    m_busy = m_loading = true;
    m_error.clear();
    emit changed();
    QMetaObject::invokeMethod(
        m_worker,
        [this, path = m_session.path(), readPins = !loaded()] {
            auto result = app::AppSession::readConfig(path, readPins);
            QMetaObject::invokeMethod(
                this,
                [this, result = std::move(result)]() mutable {
                    m_busy = m_loading = false;
                    if (result) {
                        if (auto accepted = m_session.acceptLoad(std::move(*result)); !accepted) {
                            m_error = QString::fromStdString(accepted.error());
                        }
                        sampleAnalytics();
                    } else {
                        m_error = QString::fromStdString(result.error());
                    }
                    emit changed();
                },
                Qt::QueuedConnection);
        },
        Qt::QueuedConnection);
}

void App::save() {
    startSave(false);
}
void App::saveAndClose() {
    startSave(true);
}

void App::startSave(bool closing) {
    if (m_busy || !loaded() || m_closing) {
        return;
    }
    auto request = m_session.prepareSave();
    if (!request) {
        m_error = QString::fromStdString(request.error());
        emit changed();
        return;
    }
    m_busy = true;
    m_closing = closing;
    m_error.clear();
    emit changed();
    QMetaObject::invokeMethod(
        m_worker,
        [this, path = m_session.path(), request = std::move(*request)] {
            auto result = app::AppSession::writeConfig(path, request);
            QMetaObject::invokeMethod(
                this,
                [this, result = std::move(result), revision = request.revision]() mutable {
                    m_busy = false;
                    const bool close = m_closing && result.has_value();
                    if (result) {
                        m_session.acceptSave(std::move(*result), revision);
                    } else {
                        m_error = "Save failed: " + QString::fromStdString(result.error().message);
                        m_closing = false;
                    }
                    emit changed();
                    if (close) {
                        emit shutdownReady();
                    }
                },
                Qt::QueuedConnection);
        },
        Qt::QueuedConnection);
}

void App::sampleAnalytics() {
    if (loaded()) {
        if (auto settings = m_session.state().snapshot()) {
            const auto &state = m_session.state();
            m_analytics.update(
                *settings, m_clock.elapsed(),
                {state.temperatureAvailable, state.humidityAvailable, state.brightnessAvailable});
        }
    }
}

void App::setLight(Light light, bool on) {
    if (!editable()) {
        return;
    }
    auto &lights = m_session.state().lights;
    switch (light) {
    case Light::LIVING_ROOM:
        lights.livingRoom = on;
        break;
    case Light::BEDROOM:
        lights.bedroom = on;
        break;
    case Light::KITCHEN:
        lights.kitchen = on;
        break;
    }
    edited();
}

void App::setAcOn(bool on) {
    if (!editable()) {
        return;
    }
    m_session.state().ac.on = on;
    edited();
}

void App::stepAcMode(int direction) {
    if (!editable() || direction == 0) {
        return;
    }
    auto &mode = m_session.state().ac.modeIndex;
    const int next = mode + (direction > 0 ? 1 : -1);
    if (next < 0 || next > static_cast<int>(Ac::Mode::TURBO)) {
        return;
    }
    mode = next;
    edited();
}

void App::setSpeaker(SpeakerControl control, int value) {
    if (!editable()) {
        return;
    }
    if (value < 0 || value > 100) {
        m_error = "Speaker values must be from 0 to 100";
        emit changed();
        return;
    }
    auto &speakers = m_session.state().speakers;
    switch (control) {
    case SpeakerControl::VOLUME:
        speakers.volume = value;
        break;
    case SpeakerControl::BASS:
        speakers.bass = value;
        break;
    case SpeakerControl::PITCH:
        speakers.pitch = value;
        break;
    }
    edited();
}

} // namespace smart_home::gui
