#ifndef HOME_CONFIG_H
#define HOME_CONFIG_H

#include <stdint.h>

#include "nlohmann/json.hpp"

#include <QString>

constexpr int MAX_AC_TEMP  = 30;
constexpr int MIN_AC_TEMP  = 15;
constexpr int AC_TEMP_STEP = 1;

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

    nlohmann::json toJSON()
    {
        nlohmann::json thisAsJSON =
        { "Lights", {
                { "LivingRoom", livingRoomLightOn },
                { "Bedroom", bedroomLightOn },
                { "Kitchen", kitchenLightOn },
        }};

        return thisAsJSON;
    }

    void fromJSON(const nlohmann::json& thisAsJson)
    {
        livingRoomLightOn = thisAsJson["LivingRoom"];
        bedroomLightOn = thisAsJson["Bedroom"];
        kitchenLightOn = thisAsJson["Kitchen"];
    }
};

struct Sensors
{
    int16_t temperature;
    int16_t humidity;
    int16_t brightness;

    Sensors()
    {
        temperature = 0;
        humidity = 0;
        brightness = 0;
    }

    nlohmann::json toJSON()
    {
        nlohmann::json thisAsJSON =
        { "Sensors", {
                { "Temperature", temperature },
                { "Humidity", humidity },
                { "Brightness", brightness },
        }};

        return thisAsJSON;
    }

    void fromJSON(const nlohmann::json& thisAsJson)
    {
        temperature = thisAsJson["Temperature"];
        humidity = thisAsJson["Humidity"];
        brightness = thisAsJson["Brightness"];
    }
};

enum class ACMode
{
    NORMAL = 0x00,
    FAST   = 0x01,
    TURBO  = 0x02,
    AC_MODE_CNT,
};

struct ACSettings
{
    bool    on;
    int16_t temperature;
    ACMode  mode;

    ACSettings()
    {
        on = false;
        temperature = 0;
        mode = ACMode::NORMAL;
    }

    nlohmann::json toJSON()
    {
        nlohmann::json thisAsJSON =
        { "AC", {
                { "On", on },
                { "Temperature", temperature },
                { "Mode", mode },
        }};

        return thisAsJSON;
    }

    void fromJSON(const nlohmann::json& thisAsJson)
    {
        on = thisAsJson["On"];
        temperature = thisAsJson["Temperature"];
        mode = thisAsJson["Mode"];
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

    nlohmann::json toJSON()
    {
        nlohmann::json thisAsJSON =
        { "Speakers", {
                { "Volume", volume },
                { "Bass", bass },
                { "Pitch", pitch },
        }};

        return thisAsJSON;
    }

    void fromJSON(nlohmann::json thisAsJson)
    {
        volume = thisAsJson["Volume"];
        bass = thisAsJson["Bass"];
        pitch = thisAsJson["Pitch"];
    }
};

struct HomeConfig
{
    LightSettings   lights;
    Sensors         sensors;
    ACSettings      AC;
    SpeakerSettings speakers;

    bool isDirty;

    HomeConfig()
        : lights(LightSettings()), sensors(Sensors()), AC(ACSettings()), speakers(SpeakerSettings()), isDirty(false)
    {

    }

    void onUpdate();

    void loadDirtyFlag(const nlohmann::json& thisAsJson);
    void fromJSON(const nlohmann::json& thisAsJson);
    nlohmann::json toJSON();

private:
    void readHardwareInputs();
    void sendHardwareOutputs();
};

QString ACModeToString(ACMode mode);

#endif // HOME_CONFIG_H
