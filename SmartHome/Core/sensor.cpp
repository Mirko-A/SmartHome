#include "sensor.h"

Sensor::Sensor() {}

void Sensor::initPins(uint8_t temperaturePin, uint8_t humidityPin, uint8_t brightnessPin) {
    m_Pins.temperature = temperaturePin;
    m_Pins.humidity = humidityPin;
    m_Pins.brightness = brightnessPin;

    // TODO: init pins with HAL
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
