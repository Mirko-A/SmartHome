#include "ac.h"

#include <ftxui/dom/elements.hpp>

namespace smart_home::tui::components {

ftxui::Component makeAcPanel(TuiState &state) {
    static const std::vector<std::string> modes{"NORMAL", "FAST", "TURBO"};

    auto toggle = ftxui::Checkbox("On", &state.ac.on);
    auto modeMenu = ftxui::Toggle(&modes, &state.ac.modeIndex);
    auto container = ftxui::Container::Vertical({toggle, modeMenu});
    return ftxui::Renderer(container, [container, toggle, modeMenu] {
        auto panel = ftxui::window(
            ftxui::text("[3]-AC "),
            ftxui::vbox({
                toggle->Render(),
                ftxui::separator(),
                ftxui::hbox({ftxui::text("Mode  ") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 7),
                             modeMenu->Render()}),
            }));
        if (container->Focused()) {
            panel = panel | ftxui::color(ftxui::Color::Green);
        }
        return panel;
    });
}

} // namespace smart_home::tui::components
