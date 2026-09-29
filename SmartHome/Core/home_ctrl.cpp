#include "home_ctrl.h"

std::expected<hal::GpioPin, std::string> parsePin(const nlohmann::json &config, const char *group, const char *key);

HomeControl::HomeControl(Light light, Ac ac, Sensor sensor)
    : m_LightSettings(), m_AcSettings(), m_SensorReadings(), m_SpeakerSettings(), m_Light(std::move(light)),
      m_Ac(std::move(ac)), m_Sensor(std::move(sensor)), m_Dirty(false) {}

std::expected<HomeControl, std::string> HomeControl::create(const nlohmann::json &pinCfgJson) {
    // Validate every pin before device factories start configuring hardware.
    auto livingRoomPin = parsePin(pinCfgJson, "lights", "living_room");
    if (!livingRoomPin) {
        return std::unexpected(livingRoomPin.error());
    }
    auto bedroomPin = parsePin(pinCfgJson, "lights", "bedroom");
    if (!bedroomPin) {
        return std::unexpected(bedroomPin.error());
    }
    auto kitchenPin = parsePin(pinCfgJson, "lights", "kitchen");
    if (!kitchenPin) {
        return std::unexpected(kitchenPin.error());
    }
    auto temperaturePin = parsePin(pinCfgJson, "sensors", "temperature");
    if (!temperaturePin) {
        return std::unexpected(temperaturePin.error());
    }
    auto humidityPin = parsePin(pinCfgJson, "sensors", "humidity");
    if (!humidityPin) {
        return std::unexpected(humidityPin.error());
    }
    auto brightnessPin = parsePin(pinCfgJson, "sensors", "brightness");
    if (!brightnessPin) {
        return std::unexpected(brightnessPin.error());
    }
    auto acPin1 = parsePin(pinCfgJson, "ac", "pin1");
    if (!acPin1) {
        return std::unexpected(acPin1.error());
    }
    auto acPin2 = parsePin(pinCfgJson, "ac", "pin2");
    if (!acPin2) {
        return std::unexpected(acPin2.error());
    }

    auto lightResult = Light::create(*livingRoomPin, *bedroomPin, *kitchenPin);
    if (!lightResult) {
        return std::unexpected(lightResult.error());
    }

    auto sensorResult = Sensor::create(*temperaturePin, *humidityPin, *brightnessPin);
    if (!sensorResult) {
        return std::unexpected(sensorResult.error());
    }

    auto acResult = Ac::create(*acPin1, *acPin2);
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

std::expected<hal::GpioPin, std::string> parsePin(const nlohmann::json &config, const char *group, const char *key) {
    const std::string path = std::string(group) + "." + key;
    try {
        const auto &value = config.at(group).at(key);
        if (!value.is_number_integer()) {
            return std::unexpected(path + ": expected an integer GPIO pin");
        }
        // JSON conversion to uint8_t can truncate; check its range first.
        if (value < 0 || value > 255) {
            return std::unexpected(path + ": expected an integer from 0 to 255");
        }
        auto pin = hal::GpioPin::create(value.get<uint8_t>());
        if (!pin) {
            return std::unexpected(path + ": " + pin.error());
        }
        return pin;
    } catch (const nlohmann::json::exception &error) {
        return std::unexpected(path + ": " + error.what());
    }
}
