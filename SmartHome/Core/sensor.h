#ifndef SENSOR_H
#define SENSOR_H

// #include "DHT22.h"

#include <cstdint>

class Sensor {
  public:
    enum class Type {
        TEMPERATURE = 0x00,
        HUMIDITY,
        BRIGHTNESS,
    };

    struct Pins {
        Pins(uint8_t temperature = 0U, uint8_t humidity = 0U, uint8_t brightness = 0U) {
            this->temperature = temperature;
            this->humidity = humidity;
            this->brightness = brightness;
        };

        uint8_t temperature;
        uint8_t humidity;
        uint8_t brightness;
    };

  public:
    Sensor(uint8_t temperaturePin, uint8_t humidityPin, uint8_t brightnessPin);

    Sensor() = delete;
    Sensor(const Sensor &) = delete;
    Sensor(Sensor &&) = delete;

    float read(Type type);

  private:
    Pins m_Pins;
};

#endif // SENSOR_H
