#include "sensor.h"

Sensor::Sensor(hal::GpioPin temperature, hal::GpioPin humidity, hal::GpioPin brightness)
    : m_Pins(temperature, humidity, brightness) {}

std::expected<Sensor, std::string> Sensor::create(hal::GpioPin temperaturePin, hal::GpioPin humidityPin,
                                                  hal::GpioPin brightnessPin) {
    auto gpioResult = hal::Gpio::instance();
    if (!gpioResult) {
        return std::unexpected(gpioResult.error());
    }
    hal::Gpio &gpio = gpioResult->get();

    if (auto r = gpio.setPinMode(temperaturePin, hal::PinMode::INPUT); !r) {
        return std::unexpected(r.error());
    }
    if (auto r = gpio.setPinMode(humidityPin, hal::PinMode::INPUT); !r) {
        return std::unexpected(r.error());
    }
    if (auto r = gpio.setPinMode(brightnessPin, hal::PinMode::INPUT); !r) {
        return std::unexpected(r.error());
    }

    return Sensor(temperaturePin, humidityPin, brightnessPin);
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
