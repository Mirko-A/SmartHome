#include "tui.h"

#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "home_config.h"

static ftxui::Element sensorGauge(const std::string &label, int value, int min, int max, const std::string &unit) {
    float ratio = (max > min) ? float(value - min) / float(max - min) : 0.f;
    ratio = std::max(0.f, std::min(1.f, ratio));
    return ftxui::hbox({
        ftxui::text(label) | size(ftxui::WIDTH, ftxui::EQUAL, 14),
        ftxui::text(std::to_string(value) + unit) | size(ftxui::WIDTH, ftxui::EQUAL, 7),
        ftxui::gauge(ratio) | ftxui::flex,
    });
}

int tui_main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    HomeConfig cfg;

    // Load the config from disk.
    std::ifstream cfgFileIn(CFG_JSON_FILE_PATH.c_str());
    if (!cfgFileIn.good())
        cfgFileIn.open(INI_JSON_FILE_PATH.c_str());
    if (cfgFileIn.good()) {
        nlohmann::json cfgJson;
        cfgFileIn >> cfgJson;
        cfg.fromJson(cfgJson);
    } else {
        std::cerr << "Failed to load config JSON file" << std::endl;
        return 1;
    }

    // --- AC ---
    bool acOn = cfg.m_ACSettings.on;
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
    ftxui::Component acModeMenu = ftxui::Toggle(&acModes, &acModeIndex);
    ftxui::Component acToggle = ftxui::Checkbox("On", &acOn);

    // Speaker sliders (0..100)
    ftxui::Component volumeSlider = ftxui::Slider("", &volume, 0, 100, 1);
    ftxui::Component bassSlider = ftxui::Slider("", &bass, 0, 100, 1);
    ftxui::Component pitchSlider = ftxui::Slider("", &pitch, 0, 100, 1);

    // Light checkboxes
    ftxui::Component livingRoomCheck = ftxui::Checkbox("Living Room", &livingRoomOn);
    ftxui::Component bedroomCheck = ftxui::Checkbox("Bedroom", &bedroomOn);
    ftxui::Component kitchenCheck = ftxui::Checkbox("Kitchen", &kitchenOn);

    ftxui::Component container = ftxui::Container::Vertical({
        acToggle,
        acModeMenu,
        volumeSlider,
        bassSlider,
        pitchSlider,
        livingRoomCheck,
        bedroomCheck,
        kitchenCheck,
    });

    ftxui::Component renderer = Renderer(container, [&]() -> ftxui::Element {
        // Sensor panel
        ftxui::Element sensors = window(ftxui::text(" Sensors "),
                                        ftxui::vbox({
                                            sensorGauge("Temperature", cfg.m_SensorReadings.temperature, -10, 50, " C"),
                                            sensorGauge("Humidity", cfg.m_SensorReadings.humidity, 0, 100, " %"),
                                            sensorGauge("Brightness", cfg.m_SensorReadings.brightness, 0, 1000, ""),
                                        }));

        // AC panel
        ftxui::Element ac =
            window(ftxui::text(" AC "),
                   ftxui::vbox({
                       acToggle->Render(),
                       ftxui::separator(),
                       ftxui::hbox({ftxui::text("Mode  ") | size(ftxui::WIDTH, ftxui::EQUAL, 7), acModeMenu->Render()}),
                   }));

        // Speakers panel
        ftxui::Element speakers =
            window(ftxui::text(" Speakers "),
                   ftxui::vbox({
                       ftxui::hbox({ftxui::text("Volume") | size(ftxui::WIDTH, ftxui::EQUAL, 7),
                                    volumeSlider->Render() | ftxui::flex,
                                    ftxui::text(" " + std::to_string(volume)) | size(ftxui::WIDTH, ftxui::EQUAL, 5)}),
                       ftxui::separator(ftxui::Pixel()),
                       ftxui::hbox({ftxui::text("Bass  ") | size(ftxui::WIDTH, ftxui::EQUAL, 7),
                                    bassSlider->Render() | ftxui::flex,
                                    ftxui::text(" " + std::to_string(bass)) | size(ftxui::WIDTH, ftxui::EQUAL, 5)}),
                       ftxui::separator(ftxui::Pixel()),
                       ftxui::hbox({ftxui::text("Pitch ") | size(ftxui::WIDTH, ftxui::EQUAL, 7),
                                    pitchSlider->Render() | ftxui::flex,
                                    ftxui::text(" " + std::to_string(pitch)) | size(ftxui::WIDTH, ftxui::EQUAL, 5)}),
                   }));

        ftxui::Element lights = window(ftxui::text(" Lights "), ftxui::vbox({
                                                                    livingRoomCheck->Render(),
                                                                    bedroomCheck->Render(),
                                                                    kitchenCheck->Render(),
                                                                }));

        return ftxui::vbox({
            ftxui::hbox({sensors | ftxui::flex, ac | ftxui::flex}),
            ftxui::hbox({speakers | ftxui::flex, lights | ftxui::flex}),
            ftxui::text("  Tab/arrows to navigate  Enter to toggle  q to quit") | ftxui::dim,
        });
    });

    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

    ftxui::Component with_quit = CatchEvent(renderer, [&](ftxui::Event event) {
        if (event == ftxui::Event::Character('q')) {
            // Write back to cfg before quitting
            cfg.m_ACSettings.on = acOn;
            cfg.m_ACSettings.mode = static_cast<ACMode>(acModeIndex);
            cfg.m_SpeakerSettings.volume = static_cast<int16_t>(volume);
            cfg.m_SpeakerSettings.bass = static_cast<int16_t>(bass);
            cfg.m_SpeakerSettings.pitch = static_cast<int16_t>(pitch);
            cfg.m_LightSettings.livingRoomLightOn = livingRoomOn;
            cfg.m_LightSettings.bedroomLightOn = bedroomOn;
            cfg.m_LightSettings.kitchenLightOn = kitchenOn;

            // Save the config to disk.
            nlohmann::json cfgJson = cfg.toJson();
            std::ofstream cfgFileOut(CFG_JSON_FILE_PATH.c_str());
            if (cfgFileOut.is_open()) {
                // Pretty print with 4 spaces indentation.
                cfgFileOut << cfgJson.dump(4);
                cfgFileOut.close();
            } else {
                std::cerr << "Error: Unable to open config file for writing: " << CFG_JSON_FILE_PATH << std::endl;
            }

            screen.ExitLoopClosure()();
            return true;
        }

        cfg.onUpdate();

        return false;
    });

    screen.Loop(with_quit);
    return 0;
}
