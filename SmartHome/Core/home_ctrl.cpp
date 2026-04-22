#include "home_ctrl.h"

HomeControl::HomeControl(Light light, Ac ac, Sensor sensor)
    : m_LightSettings(), m_AcSettings(), m_SensorReadings(), m_SpeakerSettings(), m_Light(std::move(light)),
      m_Ac(std::move(ac)), m_Sensor(std::move(sensor)), m_Dirty(false) {}

std::expected<HomeControl, std::string> HomeControl::create(const nlohmann::json &pinCfgJson) {
    nlohmann::json lightPinsJson = pinCfgJson["lights"];
    hal::GpioPin livingRoomPin = static_cast<hal::GpioPin>(lightPinsJson["living_room"].get<uint8_t>());
    hal::GpioPin bedroomPin = static_cast<hal::GpioPin>(lightPinsJson["bedroom"].get<uint8_t>());
    hal::GpioPin kitchenPin = static_cast<hal::GpioPin>(lightPinsJson["kitchen"].get<uint8_t>());
    auto lightResult = Light::create(livingRoomPin, bedroomPin, kitchenPin);
    if (!lightResult) {
        return std::unexpected(lightResult.error());
    }

    nlohmann::json sensorPinsJson = pinCfgJson["sensors"];
    hal::GpioPin temperaturePin = static_cast<hal::GpioPin>(sensorPinsJson["temperature"].get<uint8_t>());
    hal::GpioPin humidityPin = static_cast<hal::GpioPin>(sensorPinsJson["humidity"].get<uint8_t>());
    hal::GpioPin brightnessPin = static_cast<hal::GpioPin>(sensorPinsJson["brightness"].get<uint8_t>());
    auto sensorResult = Sensor::create(temperaturePin, humidityPin, brightnessPin);
    if (!sensorResult) {
        return std::unexpected(sensorResult.error());
    }

    nlohmann::json acPinsJson = pinCfgJson["ac"];
    hal::GpioPin acPin1 = static_cast<hal::GpioPin>(acPinsJson["pin1"].get<uint8_t>());
    hal::GpioPin acPin2 = static_cast<hal::GpioPin>(acPinsJson["pin2"].get<uint8_t>());
    auto acResult = Ac::create(acPin1, acPin2);
    if (!acResult) {
        return std::unexpected(acResult.error());
    }

    return HomeControl(std::move(*lightResult), std::move(*acResult), std::move(*sensorResult));
}

void HomeControl::loadFromJson(const nlohmann::json &json) {
    m_LightSettings.loadFromJson(json["lights"]);
    m_SensorReadings.loadFromJson(json["sensors"]);
    m_AcSettings.loadFromJson(json["ac"]);
    m_SpeakerSettings.loadFromJson(json["speakers"]);
}

nlohmann::json HomeControl::toJson() {
    nlohmann::json serialized = {
        m_LightSettings.toJson(),   m_SensorReadings.toJson(), m_AcSettings.toJson(),
        m_SpeakerSettings.toJson(), {"dirty", m_Dirty},
    };
    return serialized;
}
