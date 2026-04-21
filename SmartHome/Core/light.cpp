#include "light.h"

Light::Light() {}

void Light::initPins(uint8_t livingRoomPin, uint8_t bedroomPin, uint8_t kitchenPin) {
    m_Pins.livingRoom = livingRoomPin;
    m_Pins.bedroom = bedroomPin;
    m_Pins.kitchen = kitchenPin;

    // TODO: init pins with HAL
}

void Light::setOn(bool on, LightLocation location) {
    // TODO: set pin with HAL
}
