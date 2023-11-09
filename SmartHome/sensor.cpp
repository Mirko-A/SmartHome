#include "sensor.h"

Sensor::Sensor(const Pins& pins)
    : m_Pins(pins), m_DHT22(new TDHT22(28))
{
    // TODO: chnage TDHT22 ctor arg (pin)
    m_DHT22->Init();
}

float Sensor::read(Type type)
{
    float sensorValue;

    m_DHT22->Fetch();

    switch(type)
    {
        case Type::TEMPERATURE:
        {
            sensorValue = m_DHT22->Temp;
        }
        break;
        case Type::HUMIDITY:
        {
            sensorValue = m_DHT22->Hum;
        }
        break;
        case Type::BRIGHTNESS:
        {
            // TODO: Brightness sensor
            break;
        }
        default:
        {
        }
        break;
    }

    return sensorValue;
}
