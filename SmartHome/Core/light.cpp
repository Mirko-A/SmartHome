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

std::expected<void, std::string> Light::setOn(bool on, Light::Location location) {
    auto gpioResult = hal::Gpio::instance();
    if (!gpioResult) {
        return std::unexpected(gpioResult.error());
    }
    hal::Gpio &gpio = gpioResult->get();

    const auto state = on ? hal::PinState::HIGH : hal::PinState::LOW;
    switch (location) {
    case Light::Location::LIVING_ROOM:
        return gpio.digitalWrite(m_Pins.livingRoom, state);
    case Light::Location::BEDROOM:
        return gpio.digitalWrite(m_Pins.bedroom, state);
    case Light::Location::KITCHEN:
        return gpio.digitalWrite(m_Pins.kitchen, state);
    }
    return std::unexpected("Invalid light location");
}
