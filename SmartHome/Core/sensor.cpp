#include "sensor.h"

Sensor::Sensor() {}

std::expected<void, std::string> Sensor::initPins(hal::GpioPin temperaturePin, hal::GpioPin humidityPin,
                                                  hal::GpioPin brightnessPin) {
    m_Pins.temperature = temperaturePin;
    m_Pins.humidity = humidityPin;
    m_Pins.brightness = brightnessPin;

    if (m_Pins.temperature == hal::GpioPin::NONE || m_Pins.humidity == hal::GpioPin::NONE ||
        m_Pins.brightness == hal::GpioPin::NONE) {
        return std::unexpected("Invalid GPIO pin(s) for sensors");
    }

    hal::Gpio &gpio = hal::Gpio::instance();
    if (!gpio.isInitialized()) {
        return std::unexpected("gpio initialization failed");
    }

    gpio.pinMode(m_Pins.temperature, hal::PinMode::INPUT);
    gpio.pinMode(m_Pins.humidity, hal::PinMode::INPUT);
    gpio.pinMode(m_Pins.brightness, hal::PinMode::INPUT);
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
Sensor::Pins::Pins(hal::GpioPin temperature, hal::GpioPin humidity, hal::GpioPin brightness) {
    this->temperature = temperature;
    this->humidity = humidity;
    this->brightness = brightness;
};
