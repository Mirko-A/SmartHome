#pragma once

#include "home_settings.h"

namespace smart_home::app {

// Canonical editable state shared by both interfaces. Keep its address stable for widget bindings.
struct AppState {

    explicit AppState(const HomeSettings &settings = {})
        : lights{.livingRoom = settings.lights().livingRoomLightOn,
                 .bedroom = settings.lights().bedroomLightOn,
                 .kitchen = settings.lights().kitchenLightOn},
          speakers{.volume = settings.speakers().volume,
                   .bass = settings.speakers().bass,
                   .pitch = settings.speakers().pitch},
          ac{.on = settings.ac().on, .modeIndex = static_cast<int>(settings.ac().mode)},
          sensors{.temperature = settings.sensors().temperature,
                  .humidity = settings.sensors().humidity,
                  .brightness = settings.sensors().brightness} {}

    std::expected<HomeSettings, std::string> snapshot() const {
        HomeSettings result;
        result.setLights(lights.livingRoom, lights.bedroom, lights.kitchen);
        if (ac.modeIndex < 0 || ac.modeIndex > 2) {
            return std::unexpected("Invalid AC mode");
        }
        result.setAc(ac.on, static_cast<Ac::Mode>(ac.modeIndex));
        if (auto valid = result.setSpeakers(speakers.volume, speakers.bass, speakers.pitch);
            !valid) {
            return std::unexpected(valid.error());
        }
        result.setSensors(sensors.temperature, sensors.humidity, sensors.brightness);
        return result;
    }

    // Availability describes the latest acquisition, never values restored from disk.
    bool temperatureAvailable = false;
    bool humidityAvailable = false;
    bool brightnessAvailable = false;

    struct Lights {
        bool livingRoom;
        bool bedroom;
        bool kitchen;
    } lights;

    struct Speakers {
        int volume;
        int bass;
        int pitch;
    } speakers;

    struct AcState {
        bool on;
        int modeIndex;
    } ac;

    struct Sensors {
        int16_t temperature;
        int16_t humidity;
        int16_t brightness;
    } sensors;
};

} // namespace smart_home::app
