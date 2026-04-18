#include "tui.h"

#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <mutex>

#include "home_ctrl.h"

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

    std::mutex homeMutex;
    HomeControl home;

    // Load the config from disk.
    std::ifstream cfgFileIn(CFG_JSON_FILE_PATH.c_str());
    if (!cfgFileIn.good()) {
        cfgFileIn.open(INI_JSON_FILE_PATH.c_str());
    }
    if (cfgFileIn.good()) {
        nlohmann::json cfgJson;
        cfgFileIn >> cfgJson;
        home.fromJson(cfgJson);
    } else {
        std::cerr << "Error: Cannot open config file (in): " << CFG_JSON_FILE_PATH << " or " << INI_JSON_FILE_PATH
                  << std::endl;
        return 1;
    }

    // --- AC ---
    bool acOn = home.m_AcSettings.on;
    int acModeIndex = static_cast<int>(home.m_AcSettings.mode);

    // --- Speakers ---
    int volume = home.m_SpeakerSettings.volume;
    int bass = home.m_SpeakerSettings.bass;
    int pitch = home.m_SpeakerSettings.pitch;

    // --- Lights ---
    bool livingRoomOn = home.m_LightSettings.livingRoomLightOn;
    bool bedroomOn = home.m_LightSettings.bedroomLightOn;
    bool kitchenOn = home.m_LightSettings.kitchenLightOn;

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

    ftxui::Component acContainer = ftxui::Container::Vertical({acToggle, acModeMenu});
    ftxui::Component speakersContainer = ftxui::Container::Vertical({volumeSlider, bassSlider, pitchSlider});
    ftxui::Component lightsContainer = ftxui::Container::Vertical({livingRoomCheck, bedroomCheck, kitchenCheck});

    ftxui::Component container = ftxui::Container::Vertical({
        acContainer,
        speakersContainer,
        lightsContainer,
    });

    ftxui::Component renderer = Renderer(container, [&]() -> ftxui::Element {
        // Sensor panel
        int temp, humidity, brightness;
        {
            std::lock_guard<std::mutex> guard = std::lock_guard<std::mutex>(homeMutex);
            temp = home.m_SensorReadings.temperature;
            humidity = home.m_SensorReadings.humidity;
            brightness = home.m_SensorReadings.brightness;
        }
        ftxui::Element sensorsBox = ftxui::vbox({
            sensorGauge("Temperature", temp, -10, 50, " C"),
            sensorGauge("Humidity", humidity, 0, 100, " %"),
            sensorGauge("Brightness", brightness, 0, 1000, ""),
        });
        ftxui::Element sensors = window(ftxui::text(" Sensors "), sensorsBox);

        // AC panel
        ftxui::Element acBox = ftxui::vbox({
            acToggle->Render(),
            ftxui::separator(),
            ftxui::hbox({ftxui::text("Mode  ") | size(ftxui::WIDTH, ftxui::EQUAL, 7), acModeMenu->Render()}),
        });
        ftxui::Element ac = window(ftxui::text(" AC "), acBox);
        if (acContainer->Focused()) {
            ac = ac | ftxui::color(ftxui::Color::Green);
        }

        // Speakers panel
        ftxui::Element volumeBox = ftxui::hbox(
            {ftxui::text("Volume") | size(ftxui::WIDTH, ftxui::EQUAL, 7), volumeSlider->Render() | ftxui::flex,
             ftxui::text(" " + std::to_string(volume)) | size(ftxui::WIDTH, ftxui::EQUAL, 5)});
        ftxui::Element bassBox = ftxui::hbox(
            {ftxui::text("Bass  ") | size(ftxui::WIDTH, ftxui::EQUAL, 7), bassSlider->Render() | ftxui::flex,
             ftxui::text(" " + std::to_string(bass)) | size(ftxui::WIDTH, ftxui::EQUAL, 5)});
        ftxui::Element pitchBox = ftxui::hbox(
            {ftxui::text("Pitch ") | size(ftxui::WIDTH, ftxui::EQUAL, 7), pitchSlider->Render() | ftxui::flex,
             ftxui::text(" " + std::to_string(pitch)) | size(ftxui::WIDTH, ftxui::EQUAL, 5)});
        ftxui::Element speakersBox = ftxui::vbox({
            volumeBox,
            ftxui::separator(ftxui::Pixel()),
            bassBox,
            ftxui::separator(ftxui::Pixel()),
            pitchBox,
        });
        ftxui::Element speakers = window(ftxui::text(" Speakers "), speakersBox);
        if (speakersContainer->Focused()) {
            speakers = speakers | ftxui::color(ftxui::Color::Green);
        }

        // Lights panel
        ftxui::Element lightsBox = ftxui::vbox({
            livingRoomCheck->Render(),
            bedroomCheck->Render(),
            kitchenCheck->Render(),
        });
        ftxui::Element lights = window(ftxui::text(" Lights "), lightsBox);
        if (lightsContainer->Focused()) {
            lights = lights | ftxui::color(ftxui::Color::Green);
        }

        return ftxui::vbox({
            ftxui::hbox({sensors | ftxui::flex, ac | ftxui::flex}),
            ftxui::hbox({speakers | ftxui::flex, lights | ftxui::flex}),
            ftxui::text("  Tab/arrows to navigate  Enter to toggle  q to quit") | ftxui::dim,
        });
    });

    std::atomic<bool> done = false;
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

    std::thread ticker([&]() {
        while (!done) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            screen.PostEvent(ftxui::Event::Custom);
        }
    });

    ftxui::Component onEvent = CatchEvent(renderer, [&](ftxui::Event event) {
        if (event == ftxui::Event::Character('q')) {
            // Write back to cfg before quitting
            nlohmann::json cfgJson;
            {
                std::lock_guard<std::mutex> guard = std::lock_guard<std::mutex>(homeMutex);
                home.m_AcSettings.on = acOn;
                home.m_AcSettings.mode = static_cast<AcMode>(acModeIndex);
                home.m_SpeakerSettings.volume = static_cast<int16_t>(volume);
                home.m_SpeakerSettings.bass = static_cast<int16_t>(bass);
                home.m_SpeakerSettings.pitch = static_cast<int16_t>(pitch);
                home.m_LightSettings.livingRoomLightOn = livingRoomOn;
                home.m_LightSettings.bedroomLightOn = bedroomOn;
                home.m_LightSettings.kitchenLightOn = kitchenOn;
                cfgJson = home.toJson();
            }

            // Save the config to disk.
            std::ofstream cfgFileOut(CFG_JSON_FILE_PATH.c_str());
            if (cfgFileOut.is_open()) {
                // Pretty print with 4 spaces indentation.
                cfgFileOut << cfgJson.dump(4);
                cfgFileOut.close();
            } else {
                std::cerr << "Error: Cannot open config file (out): " << CFG_JSON_FILE_PATH << std::endl;
            }

            screen.ExitLoopClosure()();
            return true;
        }

        if (event == ftxui::Event::Custom) {
            // Simulate updates for now.
            // cfg.onUpdate();
            int delta = rand() % 5 - 2; // [-2, 2]
            {
                std::lock_guard<std::mutex> guard = std::lock_guard<std::mutex>(homeMutex);

                home.m_SensorReadings.brightness += delta;
                if (home.m_SensorReadings.brightness < 0)
                    home.m_SensorReadings.brightness = 0;
                else if (home.m_SensorReadings.brightness > 1000)
                    home.m_SensorReadings.brightness = 1000;

                home.m_SensorReadings.humidity += delta;
                if (home.m_SensorReadings.humidity < 0)
                    home.m_SensorReadings.humidity = 0;
                else if (home.m_SensorReadings.humidity > 100)
                    home.m_SensorReadings.humidity = 100;

                home.m_SensorReadings.temperature += delta;
                if (home.m_SensorReadings.temperature < -10)
                    home.m_SensorReadings.temperature = -10;
                else if (home.m_SensorReadings.temperature > 50)
                    home.m_SensorReadings.temperature = 50;
            }
        }

        return false;
    });

    screen.Loop(onEvent);
    done = true;
    ticker.join();
    return 0;
}
