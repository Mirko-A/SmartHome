#include "light.h"

#include <ftxui/dom/elements.hpp>

namespace smart_home::tui::components {

ftxui::Component makeLightsPanel(TuiState &state) {
    auto livingRoom = ftxui::Checkbox("Living Room", &state.lights.livingRoom);
    auto bedroom = ftxui::Checkbox("Bedroom", &state.lights.bedroom);
    auto kitchen = ftxui::Checkbox("Kitchen", &state.lights.kitchen);
    auto container = ftxui::Container::Vertical({livingRoom, bedroom, kitchen});
    return ftxui::Renderer(container, [container, livingRoom, bedroom, kitchen] {
        auto panel = ftxui::window(
            ftxui::text("[1]-Lights "),
            ftxui::vbox({livingRoom->Render(), bedroom->Render(), kitchen->Render()}));
        if (container->Focused()) {
            panel = panel | ftxui::color(ftxui::Color::Green);
        }
        return panel;
    });
}

} // namespace smart_home::tui::components
