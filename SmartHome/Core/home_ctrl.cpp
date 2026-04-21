#include "home_ctrl.h"

HomeControl::HomeControl()
    : m_LightSettings(), m_AcSettings(), m_SensorReadings(), m_SpeakerSettings(), m_Dirty(false) {}

void HomeControl::fromJson(const nlohmann::json &json) {
    m_LightSettings.fromJson(json["lights"]);
    m_SensorReadings.fromJson(json["sensor_readings"]);
    m_AcSettings.fromJson(json["ac"]);
    m_SpeakerSettings.fromJson(json["speakers"]);
}

void HomeControl::initPins(const nlohmann::json &pinsJson) {
    nlohmann::json lightPinsJson = pinsJson["lights"];
    nlohmann::json sensorPinsJson = pinsJson["sensors"];
    nlohmann::json acPinsJson = pinsJson["ac"];
    m_Light.initPins(lightPinsJson["living_room"], lightPinsJson["bedroom"], lightPinsJson["kitchen"]);
    m_Sensor.initPins(sensorPinsJson["temperature"], sensorPinsJson["humidity"], sensorPinsJson["brightness"]);
    m_Ac.initPins(acPinsJson["pin1"], acPinsJson["pin2"]);
}

void HomeControl::onUpdate() {
    // Inputs
    m_SensorReadings.temperature = static_cast<int16_t>(m_Sensor.read(Sensor::Type::TEMPERATURE));
    m_SensorReadings.humidity = static_cast<int16_t>(m_Sensor.read(Sensor::Type::HUMIDITY));
    m_SensorReadings.brightness = static_cast<int16_t>(m_Sensor.read(Sensor::Type::BRIGHTNESS));

    // Outputs
    m_Light.setOn(m_LightSettings.livingRoomLightOn, LightLocation::LIVING_ROOM);
    m_Light.setOn(m_LightSettings.bedroomLightOn, LightLocation::BEDROOM);
    m_Light.setOn(m_LightSettings.kitchenLightOn, LightLocation::KITCHEN);
    m_Ac.setOn(m_AcSettings.on);
    m_Ac.setMode(m_AcSettings.mode);
    m_Ac.Run();
}

void HomeControl::loadDirtyFlag(const nlohmann::json &thisAsJson) {
    m_Dirty = thisAsJson["dirty"];
}

nlohmann::json HomeControl::toJson() {
    nlohmann::json serialized = {
        m_LightSettings.toJson(),   m_SensorReadings.toJson(), m_AcSettings.toJson(),
        m_SpeakerSettings.toJson(), {"dirty", m_Dirty},
    };
    return serialized;
}

std::string AcModeToString(AcMode mode) {
    std::string modeStr;

    switch (mode) {
    case AcMode::NORMAL:
        modeStr = "Normal";
        break;
    case AcMode::FAST:
        modeStr = "Fast";
        break;
    case AcMode::TURBO:
        modeStr = "Turbo";
        break;
    default:
        modeStr = "Error";
        break;
    }

    return modeStr;
}
