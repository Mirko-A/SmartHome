#include "owned_pin.h"

#include <utility>

#include "gpio_pin.h"

#if __has_include(<wiringPi.h>)
#include <wiringPi.h>
#define HAL_HAS_WIRINGPI 1
#else
#include <random>
#define HAL_HAS_WIRINGPI 0
#endif

#include "gpio.h"

namespace hal {

OwnedPin::OwnedPin(Gpio &gpio, GpioPin pin, PinMode mode, PinState inactiveState)
    : m_Gpio(&gpio), m_Pin(pin), m_Mode(mode), m_InactiveState(inactiveState) {
#if HAL_HAS_WIRINGPI
    // Preload the output latch before enabling the digital output.
    if (m_Mode == PinMode::OUTPUT) {
        ::digitalWrite(m_Pin.number(), static_cast<int>(m_InactiveState));
    }
    ::pinMode(m_Pin.number(), static_cast<int>(m_Mode));
    if (m_Mode == PinMode::PWM_OUTPUT) {
        ::pwmWrite(m_Pin.number(), 0);
    }
#endif
}

OwnedPin::~OwnedPin() {
    shutdown();
}

OwnedPin::OwnedPin(OwnedPin &&other) noexcept
    : m_Gpio(std::exchange(other.m_Gpio, nullptr)), m_Pin(other.m_Pin), m_Mode(other.m_Mode),
      m_InactiveState(other.m_InactiveState) {}

OwnedPin &OwnedPin::operator=(OwnedPin &&other) noexcept {
    if (this != &other) {
        // Make sure any GPIO `this` already owns is released first.
        shutdown();

        m_Gpio = std::exchange(other.m_Gpio, nullptr);
        m_Pin = other.m_Pin;
        m_Mode = other.m_Mode;
        m_InactiveState = other.m_InactiveState;
    }
    return *this;
}

void OwnedPin::shutdown() noexcept {
    if (m_Gpio) {
#if HAL_HAS_WIRINGPI
        if (m_Mode == PinMode::OUTPUT) {
            ::digitalWrite(m_Pin.number(), static_cast<int>(m_InactiveState));
        } else if (m_Mode == PinMode::PWM_OUTPUT) {
            ::pwmWrite(m_Pin.number(), 0);
        }
#endif
        m_Gpio->release(m_Pin);
        m_Gpio = nullptr;
    }
}

std::expected<PinState, std::string> OwnedPin::digitalRead() {
    if (!m_Gpio) {
        return std::unexpected("Pin is not owned");
    }
    if (m_Mode != PinMode::INPUT) {
        return std::unexpected("Pin mode is not INPUT");
    }
#if HAL_HAS_WIRINGPI
    return (::digitalRead(m_Pin.number()) == 0) ? PinState::LOW : PinState::HIGH;
#else
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist(0.5);
    return dist(gen) ? PinState::HIGH : PinState::LOW;
#endif
}
std::expected<void, std::string> OwnedPin::digitalWrite(PinState state) {
    if (!m_Gpio) {
        return std::unexpected("Pin is not owned");
    }
    if (m_Mode != PinMode::OUTPUT) {
        return std::unexpected("Pin mode is not OUTPUT");
    }
#if HAL_HAS_WIRINGPI
    ::digitalWrite(m_Pin.number(), static_cast<int>(state));
#else
    (void)state;
#endif
    return std::expected<void, std::string>();
}
std::expected<int, std::string> OwnedPin::analogRead() {
    if (!m_Gpio) {
        return std::unexpected("Pin is not owned");
    }
    if (m_Mode != PinMode::INPUT) {
        return std::unexpected("Pin mode is not INPUT");
    }
#if HAL_HAS_WIRINGPI
    return ::analogRead(m_Pin.number());
#else
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 1023);
    return dist(gen);
#endif
}
std::expected<void, std::string> OwnedPin::pwmWrite(uint16_t value) {
    if (!m_Gpio) {
        return std::unexpected("Pin is not owned");
    }
    if (m_Mode != PinMode::PWM_OUTPUT) {
        return std::unexpected("Pin mode is not PWM_OUTPUT");
    }
#if HAL_HAS_WIRINGPI
    ::pwmWrite(m_Pin.number(), static_cast<int>(value));
#else
    (void)value;
#endif
    return std::expected<void, std::string>();
}

} // namespace hal
