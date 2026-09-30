#include "speaker.h"

#include <ftxui/dom/elements.hpp>
#include <string>

namespace smart_home::tui::components {
namespace {

ftxui::Element sliderRow(const std::string &label, const ftxui::Component &slider, int value) {
    return ftxui::hbox({
        ftxui::text(label) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 7),
        slider->Render() | ftxui::flex,
        ftxui::text(" " + std::to_string(value)) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 5),
    });
}

} // namespace

ftxui::Component makeSpeakersPanel(TuiState &state) {
    auto volumeSlider = ftxui::Slider("", &state.speakers.volume, 0, 100, 1);
    auto bassSlider = ftxui::Slider("", &state.speakers.bass, 0, 100, 1);
    auto pitchSlider = ftxui::Slider("", &state.speakers.pitch, 0, 100, 1);
    auto container = ftxui::Container::Vertical({volumeSlider, bassSlider, pitchSlider});
    return ftxui::Renderer(container, [container, volumeSlider, bassSlider, pitchSlider, &state] {
        auto panel = ftxui::window(ftxui::text("[2]-Speakers "),
                                   ftxui::vbox({
                                       sliderRow("Volume", volumeSlider, state.speakers.volume),
                                       ftxui::separator(ftxui::Pixel()),
                                       sliderRow("Bass  ", bassSlider, state.speakers.bass),
                                       ftxui::separator(ftxui::Pixel()),
                                       sliderRow("Pitch ", pitchSlider, state.speakers.pitch),
                                   }));
        if (container->Focused()) {
            panel = panel | ftxui::color(ftxui::Color::Green);
        }
        return panel;
    });
}

} // namespace smart_home::tui::components
