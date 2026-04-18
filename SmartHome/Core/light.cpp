#include "light.h"

Light::Light(uint8_t livingRoomPin, uint8_t bedRoomPin, uint8_t kitchenPin)
    : m_Pins(livingRoomPin, bedRoomPin, kitchenPin) {
    // TODO: init pins with HAL
}

void Light::setOn(bool on, LightLocation location) {
    // TODO: set pin with HAL
}
