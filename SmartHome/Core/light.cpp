#include "light.h"

Light::Light() {}

std::expected<void, std::string> Light::initPins(GpioPin livingRoomPin, GpioPin bedroomPin, GpioPin kitchenPin) {
    m_Pins.livingRoom = livingRoomPin;
    m_Pins.bedroom = bedroomPin;
    m_Pins.kitchen = kitchenPin;

    if (m_Pins.livingRoom == GpioPin::NONE || m_Pins.bedroom == GpioPin::NONE || m_Pins.kitchen == GpioPin::NONE) {
        return std::unexpected("Invalid GPIO pin(s) for lights");
    }

    Hal &hal = Hal::instance();
    if (!hal.isInitialized()) {
        return std::unexpected("HAL initialization failed");
    }

    hal.pinMode(m_Pins.livingRoom, PinMode::OUTPUT);
    hal.pinMode(m_Pins.bedroom, PinMode::OUTPUT);
    hal.pinMode(m_Pins.kitchen, PinMode::OUTPUT);
    return std::expected<void, std::string>();
}

std::expected<void, std::string> Light::setOn(bool on, LightLocation location) {
    GpioPin pin;
    switch (location) {
    case LightLocation::LIVING_ROOM:
        pin = m_Pins.livingRoom;
        break;
    case LightLocation::BEDROOM:
        pin = m_Pins.bedroom;
        break;
    case LightLocation::KITCHEN:
        pin = m_Pins.kitchen;
        break;
    default:
        return std::unexpected("Invalid light location");
    }

    Hal &hal = Hal::instance();
    if (!hal.isInitialized()) {
        return std::unexpected("HAL initialization failed");
    }

    hal.digitalWrite(pin, on ? PinState::HIGH : PinState::LOW);
    return std::expected<void, std::string>();
}
