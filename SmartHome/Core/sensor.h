#pragma once

#include <expected>
#include <string>

#include "owned_pin.h"

class Sensor {
  public:
    enum class Type {
        TEMPERATURE = 0x00,
        HUMIDITY,
        BRIGHTNESS,
    };

    struct Pins {
        hal::OwnedPin temperature;
        hal::OwnedPin humidity;
        hal::OwnedPin brightness;
    };

  public:
    static std::expected<Sensor, std::string>
    create(hal::GpioPin temperaturePin, hal::GpioPin humidityPin, hal::GpioPin brightnessPin);

    Sensor(const Sensor &) = delete;
    Sensor &operator=(const Sensor &) = delete;
    Sensor(Sensor &&) = default;
    Sensor &operator=(Sensor &&) = default;

    // Temperature and humidity require a DHT22 HAL operation, which is not implemented.
    std::expected<float, std::string> read(Type type);

  private:
    explicit Sensor(Pins pins);

    Pins m_Pins;
};
