#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>

#include "config_io.h"
#include "home_ctrl.h"
#include "tui_state.h"

namespace smart_home::tui {

class App {
  public:
    // The caller keeps home alive throughout the session. All settings access stays on the UI
    // thread.
    App(HomeControl &home, std::string homeConfigPath, app::ConfigSnapshot accepted);
    App(const App &) = delete;
    App &operator=(const App &) = delete;
    App(App &&) = delete;
    App &operator=(App &&) = delete;

    void run();

  private:
    ftxui::Element render();
    bool onEvent(ftxui::Event event);
    void saveAndQuit();
    void synchronizeState();
    void reload();

  private:
    std::string m_homeConfigPath;
    app::ConfigSnapshot m_accepted;
    bool m_confirmReload = false;

    // State precedes components so borrowed widget values outlive the component tree.
    // Home settings own sensor readings; UI state owns pending control edits until saveAndQuit().
    HomeControl &m_home;
    TuiState m_state;

    ftxui::ScreenInteractive m_screen;
    ftxui::Component m_lightsPanel;
    ftxui::Component m_speakersPanel;
    ftxui::Component m_acPanel;
    ftxui::Component m_root;

    std::string m_saveError;
};

} // namespace smart_home::tui
