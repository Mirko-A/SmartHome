#include "tui_app.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ftxui/dom/elements.hpp>
#include <thread>
#include <utility>

#include "components/ac.h"
#include "components/light.h"
#include "components/sensors.h"
#include "components/speaker.h"
#include "config_io.h"

namespace smart_home::tui {

TuiApp::TuiApp(HomeControl &home, std::string homeConfigPath, app::ConfigSnapshot accepted)
    : m_homeConfigPath(std::move(homeConfigPath)), m_accepted(std::move(accepted)), m_home(home),
      m_state(home.settings()), m_screen(ftxui::ScreenInteractive::Fullscreen()),
      m_lightsPanel(components::makeLightsPanel(m_state)),
      m_speakersPanel(components::makeSpeakersPanel(m_state)),
      m_acPanel(components::makeAcPanel(m_state)) {
    auto container = ftxui::Container::Vertical({m_lightsPanel, m_speakersPanel, m_acPanel});
    auto renderer = ftxui::Renderer(container, [this] { return render(); });
    m_root = ftxui::CatchEvent(renderer, [this](ftxui::Event event) { return onEvent(event); });
    m_lightsPanel->TakeFocus();
}

void TuiApp::run() {
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

ftxui::Element TuiApp::render() {
    return ftxui::vbox({
        ftxui::hbox({components::renderSensors(m_state)}),
        ftxui::hbox({m_lightsPanel->Render() | ftxui::flex, m_speakersPanel->Render() | ftxui::flex,
                     m_acPanel->Render() | ftxui::flex}),
        ftxui::text("  Tab/arrows to navigate  Enter to toggle  q to save/quit  r to reload") |
            ftxui::dim,
        ftxui::paragraph(m_saveError) | ftxui::color(ftxui::Color::Red),
    });
}

bool TuiApp::onEvent(ftxui::Event event) {
    if (m_confirmReload && event != ftxui::Event::Custom) {
        m_confirmReload = false;
        if (event == ftxui::Event::Character('y'))
            reload();
        else
            m_saveError.clear();
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

void TuiApp::saveAndQuit() {
    m_saveError.clear();
    // Copy pending UI control edits into home settings, retaining the latest sensor readings.
    auto settings = m_home.settings();
    settings.setAc(m_state.ac.on, static_cast<Ac::Mode>(m_state.ac.modeIndex));
    if (auto result = settings.setSpeakers(m_state.speakers.volume, m_state.speakers.bass,
                                           m_state.speakers.pitch);
        !result) {
        m_saveError = result.error();
        return;
    }
    settings.setLights(m_state.lights.livingRoom, m_state.lights.bedroom, m_state.lights.kitchen);
    m_home.settings() = settings;

    auto document = m_accepted.document;
    document.merge_patch(settings.serializeJson());
    if (auto result = app::saveConfig(m_homeConfigPath, document, m_accepted.version); !result) {
        m_saveError = "Save failed: " + result.error().message +
                      " Settings are still open; q retries, r reloads.";
        return;
    }
    m_screen.ExitLoopClosure()();
}

void TuiApp::reload() {
    auto result = app::loadConfig(m_homeConfigPath);
    if (!result) {
        m_saveError = result.error().message;
        return;
    }
    HomeSettings candidate;
    if (auto valid = candidate.deserializeJson(result->document); !valid) {
        m_saveError = "Invalid settings (" + m_homeConfigPath + "): " + valid.error();
        return;
    }
    m_home.settings() = candidate;
    m_state = TuiState(candidate);
    m_accepted = std::move(*result);
    m_saveError.clear();
}

void TuiApp::synchronizeState() {
    // TODO: remove randomized sensor updates.
    //
    // Synchronize sensor readings.
    int delta = std::rand() % 5 - 2;
    auto sensors = m_home.settings().sensors();
    m_home.settings().setSensors(
        static_cast<int16_t>(std::clamp(sensors.temperature + delta, -10, 50)),
        static_cast<int16_t>(std::clamp(sensors.humidity + delta, 0, 100)),
        static_cast<int16_t>(std::clamp(sensors.brightness + delta, 0, 1000)));
    sensors = m_home.settings().sensors();
    m_state.sensors = {.temperature = sensors.temperature,
                       .humidity = sensors.humidity,
                       .brightness = sensors.brightness};
}

} // namespace smart_home::tui
