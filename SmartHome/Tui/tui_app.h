#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>

#include "home_ctrl.h"
#include "tui_state.h"

namespace smart_home::tui {

class TuiApp {
  public:
    // The caller keeps home alive throughout the session. All settings access stays on the UI
    // thread.
    explicit TuiApp(HomeControl &home);
    TuiApp(const TuiApp &) = delete;
    TuiApp &operator=(const TuiApp &) = delete;
    TuiApp(TuiApp &&) = delete;
    TuiApp &operator=(TuiApp &&) = delete;

    void run();

  private:
    ftxui::Element render();
    bool onEvent(ftxui::Event event);
    void saveAndQuit();
    void synchronizeState();

    // State precedes components so borrowed widget values outlive the component tree.
    // Home settings own sensor readings; UI state owns pending control edits until saveAndQuit().
    HomeControl &m_home;
    TuiState m_state;
    std::string m_saveError;
    ftxui::ScreenInteractive m_screen;
    ftxui::Component m_lightsPanel;
    ftxui::Component m_speakersPanel;
    ftxui::Component m_acPanel;
    ftxui::Component m_root;
};

} // namespace smart_home::tui
