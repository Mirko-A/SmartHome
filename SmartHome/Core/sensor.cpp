#include "sensor.h"

Sensor::Sensor() {}

std::expected<void, std::string> Sensor::initPins(GpioPin temperaturePin, GpioPin humidityPin, GpioPin brightnessPin) {
    m_Pins.temperature = temperaturePin;
    m_Pins.humidity = humidityPin;
    m_Pins.brightness = brightnessPin;

    if (m_Pins.temperature == GpioPin::NONE || m_Pins.humidity == GpioPin::NONE || m_Pins.brightness == GpioPin::NONE) {
        return std::unexpected("Invalid GPIO pin(s) for sensors");
    }

    Hal &hal = Hal::instance();
    if (!hal.isInitialized()) {
        return std::unexpected("HAL initialization failed");
    }

    hal.pinMode(m_Pins.temperature, PinMode::INPUT);
    hal.pinMode(m_Pins.humidity, PinMode::INPUT);
    hal.pinMode(m_Pins.brightness, PinMode::INPUT);
    return std::expected<void, std::string>();
}

float Sensor::read(Type type) {
    float sensorValue = 0.0f;

    // m_DHT22->Fetch();

    switch (type) {
    case Type::TEMPERATURE: {
        // sensorValue = m_DHT22->Temp;
    } break;
    case Type::HUMIDITY: {
        // sensorValue = m_DHT22->Hum;
    } break;
    case Type::BRIGHTNESS: {
        // TODO: Brightness sensor
        break;
    }
    default: {
    } break;
    }

    return sensorValue;
}
