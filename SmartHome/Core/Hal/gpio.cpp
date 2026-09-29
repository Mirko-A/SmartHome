#include <optional>
#include <utility>

#if __has_include(<wiringPi.h>)
#include <wiringPi.h>
#define HAL_HAS_WIRINGPI 1
#else
#define HAL_HAS_WIRINGPI 0
static int wiringPiSetup() {
    return 0;
}
#endif

#include "gpio.h"

namespace hal {

Gpio::Gpio() {
    m_Initialized = (wiringPiSetup() >= 0);
    for (uint8_t number = 0; number < GpioPin::COUNT; ++number) {
        m_AvailablePins[number] = *GpioPin::create(number);
    }
}

Gpio::~Gpio() = default;

std::expected<OwnedPin, std::string> Gpio::take(GpioPin pin, PinMode mode, PinState inactiveState) {
    if (mode != PinMode::INPUT && mode != PinMode::OUTPUT && mode != PinMode::PWM_OUTPUT) {
        return std::unexpected("Unsupported pin mode");
    }
    bool isPwmPin = (pin.number() == 12 || pin.number() == 18);
    if (mode == PinMode::PWM_OUTPUT && !isPwmPin) {
        return std::unexpected("Pin does not support PWM output");
    }

    auto availablePin = std::exchange(m_AvailablePins[pin.number()], std::nullopt);
    if (!availablePin) {
        return std::unexpected("Pin already owned");
    }
    OwnedPin owned(*this, *availablePin, mode, inactiveState);
    return owned;
}

void Gpio::release(GpioPin pin) noexcept {
    m_AvailablePins[pin.number()] = pin;
}

} // namespace hal
