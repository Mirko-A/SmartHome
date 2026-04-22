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
    m_PinModes = {};
}

Gpio::~Gpio() = default;

std::expected<void, std::string> Gpio::setPinMode(GpioPin pin, PinMode mode) {
    std::size_t idx = static_cast<std::size_t>(pin);
    if (m_PinModes[idx].has_value()) {
        return std::unexpected("Pin mode already set");
    }
    bool isPwmPin = (pin == GpioPin::GPIO_12 || pin == GpioPin::GPIO_18);
    if (mode == PinMode::PWM_OUTPUT && !isPwmPin) {
        return std::unexpected("Pin does not support PWM output");
    }
#if HAL_HAS_WIRINGPI
    ::pinMode(static_cast<int>(pin), static_cast<int>(mode));
    m_PinModes[idx] = mode;
#else
    (void)mode;
#endif
    return std::expected<void, std::string>();
}

std::expected<PinState, std::string> Gpio::digitalRead(GpioPin pin) {
    std::size_t idx = static_cast<std::size_t>(pin);
    std::optional<PinMode> mode = m_PinModes[idx];
    if (!mode.has_value()) {
        return std::unexpected("Pin mode not set");
    }
    if (mode.value() != PinMode::INPUT) {
        return std::unexpected("Pin mode is not INPUT");
    }
#if HAL_HAS_WIRINGPI
    return (::digitalRead(static_cast<int>(pin)) != 0) ? PinState::HIGH : PinState::LOW;
#else
    return PinState::LOW;
#endif
}

std::expected<void, std::string> Gpio::digitalWrite(GpioPin pin, PinState state) {
    std::size_t idx = static_cast<std::size_t>(pin);
    std::optional<PinMode> mode = m_PinModes[idx];
    if (!mode.has_value()) {
        return std::unexpected("Pin mode not set");
    }
    if (mode.value() != PinMode::OUTPUT) {
        return std::unexpected("Pin mode is not OUTPUT");
    }
#if HAL_HAS_WIRINGPI
    ::digitalWrite(static_cast<int>(pin), static_cast<int>(state));
#else
    (void)state;
#endif
    return std::expected<void, std::string>();
}

std::expected<int, std::string> Gpio::analogRead(GpioPin pin) {
    std::size_t idx = static_cast<std::size_t>(pin);
    std::optional<PinMode> mode = m_PinModes[idx];
    if (!mode.has_value()) {
        return std::unexpected("Pin mode not set");
    }
    if (mode.value() != PinMode::INPUT) {
        return std::unexpected("Pin mode is not INPUT");
    }
#if HAL_HAS_WIRINGPI
    return ::analogRead(static_cast<int>(pin));
#else
    return 0;
#endif
}

std::expected<void, std::string> Gpio::pwmWrite(GpioPin pin, uint16_t value) {
    std::size_t idx = static_cast<std::size_t>(pin);
    std::optional<PinMode> mode = m_PinModes[idx];
    if (!mode.has_value()) {
        return std::unexpected("Pin mode not set");
    }
    if (mode.value() != PinMode::PWM_OUTPUT) {
        return std::unexpected("Pin mode is not PWM_OUTPUT");
    }
#if HAL_HAS_WIRINGPI
    ::pwmWrite(static_cast<int>(pin), static_cast<int>(value));
#else
    (void)value;
#endif
    return std::expected<void, std::string>();
}
} // namespace hal
