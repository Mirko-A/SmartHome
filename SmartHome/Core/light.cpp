#include "light.h"

Light::Light(const Pins& pins)
    : m_Pins(pins)
{
    // TODO: init pins with HAL
}

void Light::setOn(bool on, LightLocation location)
{
    // TODO: set pin with HAL
}
