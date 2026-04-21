#ifndef SENSOR_H
#define SENSOR_H

#include <expected>
#include <string>
// #include "DHT22.h"

#include "hal.h"

class Sensor {
  public:
    enum class Type {
        TEMPERATURE = 0x00,
        HUMIDITY,
        BRIGHTNESS,
    };

    struct Pins {
        Pins(GpioPin temperature = GpioPin::NONE, GpioPin humidity = GpioPin::NONE,
             GpioPin brightness = GpioPin::NONE) {
            this->temperature = temperature;
            this->humidity = humidity;
            this->brightness = brightness;
        };

        GpioPin temperature;
        GpioPin humidity;
        GpioPin brightness;
    };

  public:
    Sensor();

    Sensor(const Sensor &) = delete;
    Sensor(Sensor &&) = delete;

    std::expected<void, std::string> initPins(GpioPin temperaturePin, GpioPin humidityPin, GpioPin brightnessPin);

    float read(Type type);

  private:
    Pins m_Pins;
};

#endif // SENSOR_H
