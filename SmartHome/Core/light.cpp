#include "light.h"

Light::Light() {}

void Light::initPins(GpioPin livingRoomPin, GpioPin bedroomPin, GpioPin kitchenPin) {
    m_Pins.livingRoom = livingRoomPin;
    m_Pins.bedroom = bedroomPin;
    m_Pins.kitchen = kitchenPin;

    // TODO: init pins with HAL
}

void Light::setOn(bool on, LightLocation location) {
    // TODO: set pin with HAL
}
