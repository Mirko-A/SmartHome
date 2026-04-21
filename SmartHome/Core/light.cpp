#include "light.h"

Light::Light() {}

void Light::initPins(uint8_t livingRoomPin, uint8_t bedRoomPin, uint8_t kitchenPin) {
    m_Pins.livingRoom = livingRoomPin;
    m_Pins.bedRoom = bedRoomPin;
    m_Pins.kitchen = kitchenPin;

    // TODO: init pins with HAL
}

void Light::setOn(bool on, LightLocation location) {
    // TODO: set pin with HAL
}
