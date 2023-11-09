#ifndef HOME_CONFIG_H
#define HOME_CONFIG_H

#include "smart_home_types.h"
#include "light.h"
#include "sensor.h"
#include "ac.h"

#include "nlohmann/json.hpp"

#include <QString>
#include <stdint.h>

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

struct SensorReadings
{
    int16_t temperature;
    int16_t humidity;
    int16_t brightness;

    SensorReadings()
    {
        temperature = 0;
        humidity = 0;
        brightness = 0;
    }

    nlohmann::json toJSON()
    {
        nlohmann::json thisAsJSON =
        { "SensorReadings", {
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

class HomeConfig
{
public:
    HomeConfig();

    void onUpdate();

    void loadDirtyFlag(const nlohmann::json& thisAsJson);
    void fromJSON(const nlohmann::json& thisAsJson);
    nlohmann::json toJSON();

private:
    void readHardwareInputs();
    void sendHardwareOutputs();

public:
    LightSettings   m_LightSettings;
    ACSettings      m_ACSettings;
    SensorReadings  m_SensorReadings;
    SpeakerSettings m_SpeakerSettings;

private:
    Light  m_Light;
    AC     m_AC;
    Sensor m_Sensor;

public:
    bool m_Dirty;

};

QString ACModeToString(ACMode mode);

#endif // HOME_CONFIG_H
