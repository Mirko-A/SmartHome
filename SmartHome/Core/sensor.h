#ifndef SENSOR_H
#define SENSOR_H

#include <expected>
#include <string>
// #include "DHT22.h"

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

    float read(Type type);

  private:
    explicit Sensor(Pins pins);

    Pins m_Pins;
};

#endif // SENSOR_H
