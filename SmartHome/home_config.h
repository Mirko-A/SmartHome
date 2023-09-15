#ifndef HOME_CONFIG_H
#define HOME_CONFIG_H

#include <stdint.h>

#define MAX_AC_TEMP   (30)
#define MIN_AC_TEMP   (15)
#define AC_TEMP_STEP  (1)

struct LightSettings
{
    bool livingRoomLightOn;
    bool bedroomLightOn;
    bool kitchenLightOn;

    LightSettings()
    {
        livingRoomLightOn = false;
        bedroomLightOn = false;
        kitchenLightOn = false;
    }
};

struct Sensors
{
    int16_t temperature;
    int16_t brightness;
    int16_t humidity;

    Sensors()
    {
        temperature = 0;
        brightness = 0;
        humidity = 0;
    }
};

enum class ACMode
{
    NORMAL,
    TURBO,
};

struct ACSettings
{
    bool    on;
    int16_t temperature;
    ACMode  mode;

    ACSettings()
    {
        on = false;
        temperature = 20;
        mode = ACMode::NORMAL;
    }
};

struct SpeakerSettings
{
    int16_t volume;
    int16_t bass;
    int16_t pitch;

    SpeakerSettings()
    {
        volume = 0;
        bass = 0;
        pitch = 0;
    }
};

struct HomeConfig
{
    LightSettings   lights;
    Sensors         sensors;
    ACSettings      AC;
    SpeakerSettings speakers;

    HomeConfig()
        : lights(LightSettings()), sensors(Sensors()), AC(ACSettings()), speakers(SpeakerSettings())
    {

    }
};

#endif // HOME_CONFIG_H
