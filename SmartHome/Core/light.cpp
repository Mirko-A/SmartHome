#include "light.h"

Light::Light(hal::GpioPin livingRoom, hal::GpioPin bedroom, hal::GpioPin kitchen)
    : m_Pins(livingRoom, bedroom, kitchen) {}

std::expected<Light, std::string> Light::create(hal::GpioPin livingRoomPin, hal::GpioPin bedroomPin,
                                                hal::GpioPin kitchenPin) {
    auto gpioResult = hal::Gpio::instance();
    if (!gpioResult) {
        return std::unexpected(gpioResult.error());
    }
    hal::Gpio &gpio = gpioResult->get();

    if (auto r = gpio.setPinMode(livingRoomPin, hal::PinMode::OUTPUT); !r) {
        return std::unexpected(r.error());
    }
    if (auto r = gpio.setPinMode(bedroomPin, hal::PinMode::OUTPUT); !r) {
        return std::unexpected(r.error());
    }
    if (auto r = gpio.setPinMode(kitchenPin, hal::PinMode::OUTPUT); !r) {
        return std::unexpected(r.error());
    }

    return Light(livingRoomPin, bedroomPin, kitchenPin);
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

    auto gpioResult = hal::Gpio::instance();
    if (!gpioResult) {
        return std::unexpected(gpioResult.error());
    }
    hal::Gpio &gpio = gpioResult->get();

    return gpio.digitalWrite(pin, on ? hal::PinState::HIGH : hal::PinState::LOW);
}
