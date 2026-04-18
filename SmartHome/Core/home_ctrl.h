#ifndef HOME_CTRL_H
#define HOME_CTRL_H

#include <stdint.h>

#include <string>

#include "ac.h"
#include "light.h"
#include "nlohmann/json.hpp"
#include "sensor.h"
#include "smart_home_types.h"

const std::string CFG_JSON_FILE_PATH = "../resource/home_cfg.json";
const std::string INI_JSON_FILE_PATH = "../resource/ini_cfg.json";

constexpr int MAX_AC_TEMP = 30;
constexpr int MIN_AC_TEMP = 15;
constexpr int AC_TEMP_STEP = 1;

struct LightSettings {
    bool livingRoomLightOn;
    bool bedroomLightOn;
    bool kitchenLightOn;

    LightSettings() {
        livingRoomLightOn = false;
        bedroomLightOn = false;
        kitchenLightOn = false;
    }

    nlohmann::json toJson() {
        nlohmann::json thisAsJSON = {"lights",
                                     {
                                         {"living_room", livingRoomLightOn},
                                         {"bedroom", bedroomLightOn},
                                         {"kitchen", kitchenLightOn},
                                     }};

        return thisAsJSON;
    }

    void fromJson(const nlohmann::json &thisAsJson) {
        livingRoomLightOn = thisAsJson["living_room"];
        bedroomLightOn = thisAsJson["bedroom"];
        kitchenLightOn = thisAsJson["kitchen"];
    }
};

struct SensorReadings {
    int16_t temperature;
    int16_t humidity;
    int16_t brightness;

    SensorReadings() {
        temperature = 0;
        humidity = 0;
        brightness = 0;
    }

    nlohmann::json toJson() {
        nlohmann::json thisAsJSON = {"sensors",
                                     {
                                         {"temperature", temperature},
                                         {"humidity", humidity},
                                         {"brightness", brightness},
                                     }};

        return thisAsJSON;
    }

    void fromJson(const nlohmann::json &thisAsJson) {
        temperature = thisAsJson["temperature"];
        humidity = thisAsJson["humidity"];
        brightness = thisAsJson["brightness"];
    }
};

struct ASettings {
    bool on;
    AcMode mode;

    ASettings() {
        on = false;
        mode = AcMode::NORMAL;
    }

    nlohmann::json toJson() {
        nlohmann::json thisAsJSON = {"ac",
                                     {
                                         {"on", on},
                                         {"mode", mode},
                                     }};

        return thisAsJSON;
    }

    void fromJson(const nlohmann::json &thisAsJson) {
        on = thisAsJson["on"];
        mode = thisAsJson["mode"];
    }
};

struct SpeakerSettings {
    int16_t volume;
    int16_t bass;
    int16_t pitch;

    SpeakerSettings() {
        volume = 0;
        bass = 0;
        pitch = 0;
    }

    nlohmann::json toJson() {
        nlohmann::json thisAsJSON = {"speakers",
                                     {
                                         {"volume", volume},
                                         {"bass", bass},
                                         {"pitch", pitch},
                                     }};

        return thisAsJSON;
    }

    void fromJson(nlohmann::json thisAsJson) {
        volume = thisAsJson["volume"];
        bass = thisAsJson["bass"];
        pitch = thisAsJson["pitch"];
    }
};

class HomeControl {
  public:
    HomeControl();

    void onUpdate();

    void loadDirtyFlag(const nlohmann::json &thisAsJson);
    void fromJson(const nlohmann::json &thisAsJson);
    nlohmann::json toJson();

  public:
    LightSettings m_LightSettings;
    ASettings m_AcSettings;
    SensorReadings m_SensorReadings;
    SpeakerSettings m_SpeakerSettings;

  private:
    Light m_Light;
    Ac m_Ac;
    Sensor m_Sensor;

  public:
    bool m_Dirty;
};

std::string AcModeToString(AcMode mode);

#endif // HOME_CTRL_H
