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
        Pins(hal::GpioPin temperature = hal::GpioPin::NONE, hal::GpioPin humidity = hal::GpioPin::NONE,
             hal::GpioPin brightness = hal::GpioPin::NONE);

        hal::GpioPin temperature;
        hal::GpioPin humidity;
        hal::GpioPin brightness;
    };

  public:
    Sensor();

    Sensor(const Sensor &) = delete;
    Sensor(Sensor &&) = delete;

    std::expected<void, std::string> initPins(hal::GpioPin temperaturePin, hal::GpioPin humidityPin,
                                              hal::GpioPin brightnessPin);

    float read(Type type);

  private:
    Pins m_Pins;
};

#endif // SENSOR_H
