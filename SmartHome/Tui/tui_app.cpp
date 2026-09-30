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

TuiApp::TuiApp(HomeControl &home, std::string homeConfigPath)
    : m_home(home), m_homeConfigPath(std::move(homeConfigPath)), m_state(home.settings()),
      m_screen(ftxui::ScreenInteractive::Fullscreen()),
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
        ftxui::text("  Tab/arrows to navigate  Enter to toggle  q to quit") | ftxui::dim,
        ftxui::paragraph(m_saveError) | ftxui::color(ftxui::Color::Red),
    });
}

bool TuiApp::onEvent(ftxui::Event event) {
    if (event == ftxui::Event::Character('1')) {
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

    if (auto result = saveConfig(m_homeConfigPath, m_home.serializeJson()); !result) {
        m_saveError = "Save failed: " + result.error() + " (" + m_homeConfigPath +
                      "). Settings are still open; press q to retry.";
        return;
    }
    m_screen.ExitLoopClosure()();
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
