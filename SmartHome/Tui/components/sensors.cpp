#include "sensors.h"

#include <algorithm>
#include <string>

namespace smart_home::tui::components {
namespace {

ftxui::Element sensorGauge(const std::string &label, int value, int min, int max,
                           const std::string &unit) {
    float ratio = (max > min) ? float(value - min) / float(max - min) : 0.f;
    ratio = std::max(0.f, std::min(1.f, ratio));
    return ftxui::hbox({
        ftxui::text(label) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 14),
        ftxui::text(std::to_string(value) + unit) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 7),
        ftxui::gauge(ratio) | ftxui::flex,
    });
}

} // namespace

ftxui::Element renderSensors(const TuiState &state) {
    return ftxui::window(ftxui::text(" Sensors "),
                         ftxui::vbox({
                             sensorGauge("Temperature", state.sensors.temperature, -10, 50, " C"),
                             sensorGauge("Humidity", state.sensors.humidity, 0, 100, " %"),
                             sensorGauge("Brightness", state.sensors.brightness, 0, 1000, ""),
                         }));
}

} // namespace smart_home::tui::components
