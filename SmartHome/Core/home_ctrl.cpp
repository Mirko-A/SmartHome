#include "home_ctrl.h"

HomeControl::HomeControl()
    : m_LightSettings(), m_AcSettings(), m_SensorReadings(), m_SpeakerSettings(), m_Dirty(false) {}

void HomeControl::fromJson(const nlohmann::json &json) {
    m_LightSettings.fromJson(json["lights"]);
    m_SensorReadings.fromJson(json["sensors"]);
    m_AcSettings.fromJson(json["ac"]);
    m_SpeakerSettings.fromJson(json["speakers"]);
}

std::expected<void, std::string> HomeControl::initPins(const nlohmann::json &pinCfgJson) {
    nlohmann::json lightPinsJson = pinCfgJson["lights"];
    hal::GpioPin livingRoomPin = static_cast<hal::GpioPin>(lightPinsJson["living_room"].get<uint8_t>());
    hal::GpioPin bedroomPin = static_cast<hal::GpioPin>(lightPinsJson["bedroom"].get<uint8_t>());
    hal::GpioPin kitchenPin = static_cast<hal::GpioPin>(lightPinsJson["kitchen"].get<uint8_t>());
    if (auto result = m_Light.initPins(livingRoomPin, bedroomPin, kitchenPin); !result) {
        return std::unexpected(result.error());
    }

    nlohmann::json sensorPinsJson = pinCfgJson["sensors"];
    hal::GpioPin temperaturePin = static_cast<hal::GpioPin>(sensorPinsJson["temperature"].get<uint8_t>());
    hal::GpioPin humidityPin = static_cast<hal::GpioPin>(sensorPinsJson["humidity"].get<uint8_t>());
    hal::GpioPin brightnessPin = static_cast<hal::GpioPin>(sensorPinsJson["brightness"].get<uint8_t>());
    if (auto result = m_Sensor.initPins(temperaturePin, humidityPin, brightnessPin); !result) {
        return std::unexpected(result.error());
    }

    nlohmann::json acPinsJson = pinCfgJson["ac"];
    hal::GpioPin acPin1 = static_cast<hal::GpioPin>(acPinsJson["pin1"].get<uint8_t>());
    hal::GpioPin acPin2 = static_cast<hal::GpioPin>(acPinsJson["pin2"].get<uint8_t>());
    if (auto result = m_Ac.initPins(acPin1, acPin2); !result) {
        return std::unexpected(result.error());
    }

    return std::expected<void, std::string>();
}

std::expected<void, std::string> HomeControl::onUpdate() {
    // Inputs
    m_SensorReadings.temperature = static_cast<int16_t>(m_Sensor.read(Sensor::Type::TEMPERATURE));
    m_SensorReadings.humidity = static_cast<int16_t>(m_Sensor.read(Sensor::Type::HUMIDITY));
    m_SensorReadings.brightness = static_cast<int16_t>(m_Sensor.read(Sensor::Type::BRIGHTNESS));

    std::expected<void, std::string> result;

    // Outputs
    result = m_Light.setOn(m_LightSettings.livingRoomLightOn, LightLocation::LIVING_ROOM);
    if (!result) {
        return std::unexpected(result.error());
    }

    result = m_Light.setOn(m_LightSettings.bedroomLightOn, LightLocation::BEDROOM);
    if (!result) {
        return std::unexpected(result.error());
    }

    result = m_Light.setOn(m_LightSettings.kitchenLightOn, LightLocation::KITCHEN);
    if (!result) {
        return std::unexpected(result.error());
    }

    m_Ac.setOn(m_AcSettings.on);
    m_Ac.setMode(m_AcSettings.mode);
    m_Ac.Run();

    return std::expected<void, std::string>();
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
