#ifndef SENSOR_H
#define SENSOR_H

#include "DHT22.h"

#include <cstdint>

class Sensor
{
public:
    enum class Type
    {
        TEMPERATURE = 0x00,
        HUMIDITY,
        BRIGHTNESS,
        TYPE_CNT,
    };

    struct Pins
    {
        // TODO: add pins
        Pins(uint8_t temperature = 0U, uint8_t humidity = 0U, uint8_t brightness = 0U)
        {
            this->temperature = temperature;
            this->humidity    = humidity;
            this->brightness  = brightness;
        };

        Pins(const Pins& other)
        {
            temperature = other.temperature;
            humidity    = other.humidity;
            brightness  = other.brightness;
        };

        uint8_t temperature;
        uint8_t humidity;
        uint8_t brightness;
    };

public:
    Sensor(const Pins& pins);

    float read(Type type);

private:
    Pins m_Pins;
    TDHT22* m_DHT22;
};

#endif // SENSOR_H
