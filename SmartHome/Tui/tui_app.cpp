#include "tui_app.h"

#include <chrono>
#include <cstdint>
#include <ftxui/dom/elements.hpp>
#include <thread>
#include <utility>

#include "components/ac.h"
#include "components/light.h"
#include "components/sensors.h"
#include "components/speaker.h"

namespace smart_home::tui {

App::App(app::AppSession &session)
    : m_session(session), m_screen(ftxui::ScreenInteractive::Fullscreen()),
      m_lightsPanel(components::makeLightsPanel(session.state())),
      m_speakersPanel(components::makeSpeakersPanel(session.state())),
      m_acPanel(components::makeAcPanel(session.state())) {
    auto container = ftxui::Container::Vertical({m_lightsPanel, m_speakersPanel, m_acPanel});
    auto renderer = ftxui::Renderer(container, [this] { return render(); });
    m_root = ftxui::CatchEvent(renderer, [this, renderer](ftxui::Event event) {
        if (onEvent(event)) {
            return true;
        }
        if (renderer->OnEvent(event)) {
            m_session.edited();
        }
        return true;
    });
    m_lightsPanel->TakeFocus();
}

void App::run() {
    std::jthread ticker([this](std::stop_token stop) {
        while (!stop.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (!stop.stop_requested()) {
                m_screen.PostEvent(ftxui::Event::Custom);
            }
        }
    });
    m_screen.Loop(m_root);
}

ftxui::Element App::render() {
    return ftxui::vbox({
        ftxui::hbox({components::renderSensors(m_session.state())}),
        ftxui::hbox({m_lightsPanel->Render() | ftxui::flex, m_speakersPanel->Render() | ftxui::flex,
                     m_acPanel->Render() | ftxui::flex}),
        ftxui::text("  Tab/arrows to navigate  Enter to toggle  q to save/quit  r to reload") |
            ftxui::dim,
        ftxui::paragraph(m_session.deviceStatus()) | ftxui::dim,
        ftxui::paragraph(m_saveError) | ftxui::color(ftxui::Color::Red),
    });
}

bool App::onEvent(ftxui::Event event) {
    if (m_confirmReload && event != ftxui::Event::Custom) {
        m_confirmReload = false;
        if (event == ftxui::Event::Character('y')) {
            reload();
        } else {
            m_saveError.clear();
        }
        return true;
    }
    if (event == ftxui::Event::Character('r')) {
        m_confirmReload = true;
        m_saveError =
            "Discard pending edits and reload? Press y to confirm, any other key to cancel.";
    } else if (event == ftxui::Event::Character('1')) {
        m_lightsPanel->TakeFocus();
    } else if (event == ftxui::Event::Character('2')) {
        m_speakersPanel->TakeFocus();
    } else if (event == ftxui::Event::Character('3')) {
        m_acPanel->TakeFocus();
    } else if (event == ftxui::Event::Character('q')) {
        saveAndQuit();
    } else if (event == ftxui::Event::Custom) {
        synchronizeState();
    } else {
        return false;
    }
    return true;
}

void App::saveAndQuit() {
    m_saveError.clear();
    if (auto result = m_session.save(); !result) {
        m_saveError =
            "Save failed: " + result.error() + " Settings are still open; q retries, r reloads.";
        return;
    }
    m_screen.ExitLoopClosure()();
}

void App::reload() {
    if (auto result = m_session.reload(); !result) {
        m_saveError = result.error();
        return;
    }
    m_saveError.clear();
}

void App::synchronizeState() {
    m_session.update();
}

} // namespace smart_home::tui
