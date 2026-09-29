#include "light.h"

#include <hal.h>

Light::Light(Pins pins) : m_Pins(std::move(pins)) {}

std::expected<Light, std::string> Light::create(hal::GpioPin livingRoomPin, hal::GpioPin bedroomPin,
                                                hal::GpioPin kitchenPin) {
    auto gpioResult = hal::Gpio::instance();
    if (!gpioResult) {
        return std::unexpected(gpioResult.error());
    }
    hal::Gpio &gpio = gpioResult->get();

    auto livingRoomResult = gpio.take(livingRoomPin, hal::PinMode::OUTPUT);
    if (!livingRoomResult) {
        return std::unexpected(livingRoomResult.error());
    }
    auto bedroomResult = gpio.take(bedroomPin, hal::PinMode::OUTPUT);
    if (!bedroomResult) {
        return std::unexpected(bedroomResult.error());
    }
    auto kitchenResult = gpio.take(kitchenPin, hal::PinMode::OUTPUT);
    if (!kitchenResult) {
        return std::unexpected(kitchenResult.error());
    }

    return Light(
        Pins{std::move(*livingRoomResult), std::move(*bedroomResult), std::move(*kitchenResult)});
}

std::expected<void, std::string> Light::setOn(bool on, Light::Location location) {
    const auto state = on ? hal::PinState::HIGH : hal::PinState::LOW;
    switch (location) {
    case Light::Location::LIVING_ROOM:
        return m_Pins.livingRoom.digitalWrite(state);
    case Light::Location::BEDROOM:
        return m_Pins.bedroom.digitalWrite(state);
    case Light::Location::KITCHEN:
        return m_Pins.kitchen.digitalWrite(state);
    }
    return std::unexpected("Invalid light location");
}
