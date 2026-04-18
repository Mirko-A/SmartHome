#include "home_ctrl.h"

HomeControl::HomeControl()
    : m_LightSettings(LightSettings()), m_ACSettings(ACSettings()), m_SensorReadings(SensorReadings()),
      m_SpeakerSettings(SpeakerSettings()), m_Light(Light::Pins()), m_AC(AC::Pins()), m_Sensor(Sensor::Pins()),
      m_Dirty(false) {}

void HomeControl::readHardwareInputs() {
    // Sensors
    m_SensorReadings.temperature = static_cast<int16_t>(m_Sensor.read(Sensor::Type::TEMPERATURE));
    m_SensorReadings.humidity = static_cast<int16_t>(m_Sensor.read(Sensor::Type::HUMIDITY));
    m_SensorReadings.brightness = static_cast<int16_t>(m_Sensor.read(Sensor::Type::BRIGHTNESS));
}

void HomeControl::sendHardwareOutputs() {
    // Lights
    m_Light.setOn(m_LightSettings.livingRoomLightOn, LightLocation::LIVING_ROOM);
    m_Light.setOn(m_LightSettings.bedroomLightOn, LightLocation::BEDROOM);
    m_Light.setOn(m_LightSettings.kitchenLightOn, LightLocation::KITCHEN);
    // AC
    m_AC.setOn(m_ACSettings.on);
    m_AC.setMode(m_ACSettings.mode);
    m_AC.Run();
}

void HomeControl::onUpdate() {
    readHardwareInputs();
    sendHardwareOutputs();
}

void HomeControl::fromJson(const nlohmann::json &thisAsJson) {
    m_LightSettings.fromJson(thisAsJson["Lights"]);
    m_SensorReadings.fromJson(thisAsJson["SensorReadings"]);
    m_ACSettings.fromJson(thisAsJson["AC"]);
    m_SpeakerSettings.fromJson(thisAsJson["Speakers"]);
}

void HomeControl::loadDirtyFlag(const nlohmann::json &thisAsJson) {
    m_Dirty = thisAsJson["Dirty"];
}

nlohmann::json HomeControl::toJson() {
    nlohmann::json thisAsJSON = {
        m_LightSettings.toJson(),   m_SensorReadings.toJson(), m_ACSettings.toJson(),
        m_SpeakerSettings.toJson(), {"Dirty", m_Dirty},
    };

    return thisAsJSON;
}

std::string ACModeToString(ACMode mode) {
    std::string modeStr;

    switch (mode) {
    case ACMode::NORMAL:
        modeStr = "Normal";
        break;
    case ACMode::FAST:
        modeStr = "Fast";
        break;
    case ACMode::TURBO:
        modeStr = "Turbo";
        break;
    default:
        modeStr = "Error";
        break;
    }

    return modeStr;
}
