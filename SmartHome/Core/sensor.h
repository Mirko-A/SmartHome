#ifndef SENSOR_H
#define SENSOR_H

#include <expected>
#include <string>
// #include "DHT22.h"

#include "gpio.h"

class Sensor {
  public:
    enum class Type {
        TEMPERATURE = 0x00,
        HUMIDITY,
        BRIGHTNESS,
    };

    struct Pins {
        Pins(hal::GpioPin temperature, hal::GpioPin humidity, hal::GpioPin brightness);

        hal::GpioPin temperature;
        hal::GpioPin humidity;
        hal::GpioPin brightness;
    };

  public:
    static std::expected<Sensor, std::string> create(hal::GpioPin temperaturePin, hal::GpioPin humidityPin,
                                                     hal::GpioPin brightnessPin);

    Sensor(const Sensor &) = delete;
    Sensor &operator=(const Sensor &) = delete;
    Sensor(Sensor &&) = default;
    Sensor &operator=(Sensor &&) = default;

    float read(Type type);

  private:
    Sensor(hal::GpioPin temperature, hal::GpioPin humidity, hal::GpioPin brightness);

    Pins m_Pins;
};

#endif // SENSOR_H
