#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>

#include "app_session.h"

namespace smart_home::tui {

class App {
  public:
    // The caller keeps the shared session alive; all device access stays on the UI thread.
    explicit App(app::AppSession &session);
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
    bool m_confirmReload = false;

    app::AppSession &m_session;

    ftxui::ScreenInteractive m_screen;
    ftxui::Component m_lightsPanel;
    ftxui::Component m_speakersPanel;
    ftxui::Component m_acPanel;
    ftxui::Component m_root;

    std::string m_saveError;
};

} // namespace smart_home::tui
