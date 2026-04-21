#ifndef HOME_CTRL_H
#define HOME_CTRL_H

#include <stdint.h>

#include <string>

#include "ac.h"
#include "light.h"
#include "nlohmann/json.hpp"
#include "sensor.h"
#include "smart_home_types.h"

const std::string HOME_INI_FILE_PATH = "../resource/ini_cfg.json";
const std::string HOME_CFG_FILE_PATH = "../resource/home_cfg.json";
const std::string PIN_CFG_FILE_PATH = "../resource/pin_cfg.json";

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
        nlohmann::json json = {"lights",
                               {
                                   {"living_room", livingRoomLightOn},
                                   {"bedroom", bedroomLightOn},
                                   {"kitchen", kitchenLightOn},
                               }};
        return json;
    }

    void fromJson(const nlohmann::json &json) {
        livingRoomLightOn = json["living_room"];
        bedroomLightOn = json["bedroom"];
        kitchenLightOn = json["kitchen"];
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
        nlohmann::json json = {"sensors",
                               {
                                   {"temperature", temperature},
                                   {"humidity", humidity},
                                   {"brightness", brightness},
                               }};
        return json;
    }

    void fromJson(const nlohmann::json &json) {
        temperature = json["temperature"];
        humidity = json["humidity"];
        brightness = json["brightness"];
    }
};

struct AcSettings {
    bool on;
    AcMode mode;

    AcSettings() {
        on = false;
        mode = AcMode::NORMAL;
    }

    nlohmann::json toJson() {
        nlohmann::json json = {"ac",
                               {
                                   {"on", on},
                                   {"mode", mode},
                               }};
        return json;
    }

    void fromJson(const nlohmann::json &json) {
        on = json["on"];
        mode = json["mode"];
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
        nlohmann::json json = {"speakers",
                               {
                                   {"volume", volume},
                                   {"bass", bass},
                                   {"pitch", pitch},
                               }};
        return json;
    }

    void fromJson(nlohmann::json json) {
        volume = json["volume"];
        bass = json["bass"];
        pitch = json["pitch"];
    }
};

class HomeControl {
  public:
    HomeControl();
    void initPins(const nlohmann::json &pinsJson);

    void onUpdate();

    void loadDirtyFlag(const nlohmann::json &thisAsJson);
    void fromJson(const nlohmann::json &json);
    nlohmann::json toJson();

  public:
    LightSettings m_LightSettings;
    AcSettings m_AcSettings;
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
