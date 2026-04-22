#include "light.h"

Light::Light() {}

std::expected<void, std::string> Light::initPins(hal::GpioPin livingRoomPin, hal::GpioPin bedroomPin,
                                                 hal::GpioPin kitchenPin) {
    m_Pins.livingRoom = livingRoomPin;
    m_Pins.bedroom = bedroomPin;
    m_Pins.kitchen = kitchenPin;

    if (m_Pins.livingRoom == hal::GpioPin::NONE || m_Pins.bedroom == hal::GpioPin::NONE ||
        m_Pins.kitchen == hal::GpioPin::NONE) {
        return std::unexpected("Invalid GPIO pin(s) for lights");
    }

    hal::Gpio &gpio = hal::Gpio::instance();
    if (!gpio.isInitialized()) {
        return std::unexpected("gpio initialization failed");
    }

    gpio.pinMode(m_Pins.livingRoom, hal::PinMode::OUTPUT);
    gpio.pinMode(m_Pins.bedroom, hal::PinMode::OUTPUT);
    gpio.pinMode(m_Pins.kitchen, hal::PinMode::OUTPUT);
    return std::expected<void, std::string>();
}

std::expected<void, std::string> Light::setOn(bool on, LightLocation location) {
    hal::GpioPin pin;
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

    hal::Gpio &gpio = hal::Gpio::instance();
    if (!gpio.isInitialized()) {
        return std::unexpected("gpio initialization failed");
    }

    gpio.digitalWrite(pin, on ? hal::PinState::HIGH : hal::PinState::LOW);
    return std::expected<void, std::string>();
}
