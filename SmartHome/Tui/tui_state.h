#pragma once

#include "home_settings.h"

namespace smart_home::tui {

// FTXUI borrows pointers into this draft; keep its address stable for the UI session.
struct TuiState {
    // Copy loaded settings once at startup; sensor readings are refreshed during the session.
    explicit TuiState(const HomeSettings &settings)
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

    struct LightsTui {
        bool livingRoom;
        bool bedroom;
        bool kitchen;
    } lights;

    struct SpeakersTui {
        int volume;
        int bass;
        int pitch;
    } speakers;

    struct AcTui {
        bool on;
        int modeIndex;
    } ac;

    struct SensorsTui {
        int temperature;
        int humidity;
        int brightness;
    } sensors;
};

} // namespace smart_home::tui
