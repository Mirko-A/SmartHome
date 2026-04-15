#include "tui.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "home_config.h"

using namespace ftxui;

static Element sensorGauge(const std::string &label, int value, int min, int max, const std::string &unit) {
    float ratio = (max > min) ? float(value - min) / float(max - min) : 0.f;
    ratio = std::max(0.f, std::min(1.f, ratio));
    return hbox({
        text(label) | size(WIDTH, EQUAL, 14),
        text(std::to_string(value) + unit) | size(WIDTH, EQUAL, 7),
        gauge(ratio) | flex,
    });
}

int tui_main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    HomeConfig cfg;

    // --- AC ---
    bool acOn = cfg.m_ACSettings.on;
    int acTemp = cfg.m_ACSettings.temperature;
    int acModeIndex = static_cast<int>(cfg.m_ACSettings.mode);

    // --- Speakers ---
    int volume = cfg.m_SpeakerSettings.volume;
    int bass = cfg.m_SpeakerSettings.bass;
    int pitch = cfg.m_SpeakerSettings.pitch;

    // --- Lights ---
    bool livingRoomOn = cfg.m_LightSettings.livingRoomLightOn;
    bool bedroomOn = cfg.m_LightSettings.bedroomLightOn;
    bool kitchenOn = cfg.m_LightSettings.kitchenLightOn;

    // AC temperature slider (MIN_AC_TEMP..MAX_AC_TEMP)
    std::vector<std::string> acModes = {"Normal", "Fast", "Turbo"};
    Component acModeMenu = Toggle(&acModes, &acModeIndex);
    Component acTempSlider = Slider("", &acTemp, MIN_AC_TEMP, MAX_AC_TEMP, AC_TEMP_STEP);
    Component acToggle = Checkbox("On", &acOn);

    // Speaker sliders (0..100)
    Component volumeSlider = Slider("", &volume, 0, 100, 1);
    Component bassSlider = Slider("", &bass, 0, 100, 1);
    Component pitchSlider = Slider("", &pitch, 0, 100, 1);

    // Light checkboxes
    Component livingRoomCheck = Checkbox("Living Room", &livingRoomOn);
    Component bedroomCheck = Checkbox("Bedroom", &bedroomOn);
    Component kitchenCheck = Checkbox("Kitchen", &kitchenOn);

    Component container = Container::Vertical({
        acToggle,
        acTempSlider,
        acModeMenu,
        volumeSlider,
        bassSlider,
        pitchSlider,
        livingRoomCheck,
        bedroomCheck,
        kitchenCheck,
    });

    Component renderer = Renderer(container, [&]() -> Element {
        // Sensor panel
        Element sensors =
            window(text(" Sensors "), vbox({
                                          sensorGauge("Temperature", cfg.m_SensorReadings.temperature, -10, 50, " C"),
                                          sensorGauge("Humidity", cfg.m_SensorReadings.humidity, 0, 100, " %"),
                                          sensorGauge("Brightness", cfg.m_SensorReadings.brightness, 0, 1000, ""),
                                      }));

        // AC panel
        Element ac =
            window(text(" AC "), vbox({
                                     acToggle->Render(),
                                     separator(),
                                     hbox({text("Temp  ") | size(WIDTH, EQUAL, 7), acTempSlider->Render() | flex,
                                           text(" " + std::to_string(acTemp) + " C") | size(WIDTH, EQUAL, 6)}),
                                     hbox({text("Mode  ") | size(WIDTH, EQUAL, 7), acModeMenu->Render()}),
                                 }));

        // Speakers panel
        Element speakers =
            window(text(" Speakers "), vbox({
                                           hbox({text("Volume") | size(WIDTH, EQUAL, 7), volumeSlider->Render() | flex,
                                                 text(" " + std::to_string(volume)) | size(WIDTH, EQUAL, 5)}),
                                           hbox({text("Bass  ") | size(WIDTH, EQUAL, 7), bassSlider->Render() | flex,
                                                 text(" " + std::to_string(bass)) | size(WIDTH, EQUAL, 5)}),
                                           hbox({text("Pitch ") | size(WIDTH, EQUAL, 7), pitchSlider->Render() | flex,
                                                 text(" " + std::to_string(pitch)) | size(WIDTH, EQUAL, 5)}),
                                       }));

        // Lights panel
        Element lights = window(text(" Lights "), vbox({
                                                      livingRoomCheck->Render(),
                                                      bedroomCheck->Render(),
                                                      kitchenCheck->Render(),
                                                  }));

        return vbox({
            hbox({sensors | flex, ac | flex}),
            hbox({speakers | flex, lights | flex}),
            text("  Tab/arrows to navigate  Enter to toggle  q to quit") | dim,
        });
    });

    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    Component with_quit = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            // Write back to cfg before quitting
            cfg.m_ACSettings.on = acOn;
            cfg.m_ACSettings.temperature = static_cast<int16_t>(acTemp);
            cfg.m_ACSettings.mode = static_cast<ACMode>(acModeIndex);
            cfg.m_SpeakerSettings.volume = static_cast<int16_t>(volume);
            cfg.m_SpeakerSettings.bass = static_cast<int16_t>(bass);
            cfg.m_SpeakerSettings.pitch = static_cast<int16_t>(pitch);
            cfg.m_LightSettings.livingRoomLightOn = livingRoomOn;
            cfg.m_LightSettings.bedroomLightOn = bedroomOn;
            cfg.m_LightSettings.kitchenLightOn = kitchenOn;
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(with_quit);
    return 0;
}
