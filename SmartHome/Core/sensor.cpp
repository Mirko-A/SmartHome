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

std::expected<float, std::string> Sensor::read(Type type) {
    switch (type) {
    case Type::TEMPERATURE:
        return std::unexpected("Temperature reading is unsupported (DHT22 HAL unavailable)");
    case Type::HUMIDITY:
        return std::unexpected("Humidity reading is unsupported (DHT22 HAL unavailable)");
    case Type::BRIGHTNESS: {
        auto reading = m_Pins.brightness.analogRead();
        if (!reading) {
            return std::unexpected(reading.error());
        }
        return static_cast<float>(*reading);
    }
    }
    return std::unexpected("Invalid sensor type");
}
