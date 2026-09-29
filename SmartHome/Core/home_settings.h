#ifndef HOME_SETTINGS_H
#define HOME_SETTINGS_H

#include <cstdint>
#include <expected>
#include <string>

#include "ac.h"
#include "nlohmann/json.hpp"

// Readable snapshots; modifying these values cannot change stored settings.
struct LightSettings {
    bool livingRoomLightOn = false;
    bool bedroomLightOn = false;
    bool kitchenLightOn = false;
};

struct SensorReadings {
    int16_t temperature = 0;
    int16_t humidity = 0;
    int16_t brightness = 0;
};

struct AcSettings {
    bool on = false;
    Ac::Mode mode = Ac::Mode::NORMAL;
};

struct SpeakerSettings {
    int16_t volume = 0;
    int16_t bass = 0;
    int16_t pitch = 0;
};

class HomeSettings {
  public:
    LightSettings lights() const {
        return m_Lights;
    }
    AcSettings ac() const {
        return m_Ac;
    }
    SensorReadings sensors() const {
        return m_Sensors;
    }
    SpeakerSettings speakers() const {
        return m_Speakers;
    }

    void setLights(LightSettings lights) {
        m_Lights = lights;
    }
    void setSensors(SensorReadings sensors) {
        m_Sensors = sensors;
    }
    std::expected<void, std::string> setAc(bool on, Ac::Mode mode);
    // Speaker controls use the same 0..100 range as the TUI sliders.
    std::expected<void, std::string> setSpeakers(int volume, int bass, int pitch);

    std::expected<void, std::string> loadFromJson(const nlohmann::json &json);
    nlohmann::json toJson() const;

  private:
    LightSettings m_Lights;
    AcSettings m_Ac;
    SensorReadings m_Sensors;
    SpeakerSettings m_Speakers;
};

#endif // HOME_SETTINGS_H
