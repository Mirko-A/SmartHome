#include "sensor.h"

#include "gpio.h"

Sensor::Sensor(Pins pins) : m_Pins(std::move(pins)) {}

std::expected<Sensor, std::string>
Sensor::create(hal::GpioPin temperaturePin, hal::GpioPin humidityPin, hal::GpioPin brightnessPin) {
    auto gpioResult = hal::Gpio::instance();
    if (!gpioResult) {
        return std::unexpected(gpioResult.error());
    }
    hal::Gpio &gpio = gpioResult->get();

    auto temperatureResult = gpio.take(temperaturePin, hal::PinMode::INPUT);
    if (!temperatureResult) {
        return std::unexpected(temperatureResult.error());
    }
    auto humidityResult = gpio.take(humidityPin, hal::PinMode::INPUT);
    if (!humidityResult) {
        return std::unexpected(humidityResult.error());
    }
    auto brightnessResult = gpio.take(brightnessPin, hal::PinMode::INPUT);
    if (!brightnessResult) {
        return std::unexpected(brightnessResult.error());
    }

    return Sensor(Pins{std::move(*temperatureResult), std::move(*humidityResult),
                       std::move(*brightnessResult)});
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
