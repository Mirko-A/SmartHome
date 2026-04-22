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
}

Gpio::~Gpio() = default;

void Gpio::pinMode(GpioPin pin, PinMode mode) {
    if (pin == GpioPin::NONE) {
        return;
    }
#if HAL_HAS_WIRINGPI
    ::pinMode(static_cast<int>(pin), static_cast<int>(mode));
#else
    (void)mode;
#endif
}

PinState Gpio::digitalRead(GpioPin pin) {
    if (pin == GpioPin::NONE) {
        return PinState::LOW;
    }
#if HAL_HAS_WIRINGPI
    return (::digitalRead(static_cast<int>(pin)) != 0) ? PinState::HIGH : PinState::LOW;
#else
    return PinState::LOW;
#endif
}

void Gpio::digitalWrite(GpioPin pin, PinState state) {
    if (pin == GpioPin::NONE) {
        return;
    }
#if HAL_HAS_WIRINGPI
    ::digitalWrite(static_cast<int>(pin), static_cast<int>(state));
#else
    (void)state;
#endif
}

int Gpio::analogRead(GpioPin pin) {
    if (pin == GpioPin::NONE) {
        return 0;
    }
#if HAL_HAS_WIRINGPI
    return ::analogRead(static_cast<int>(pin));
#else
    return 0;
#endif
}

void Gpio::pwmWrite(GpioPin pin, uint16_t value) {
    if (pin == GpioPin::NONE) {
        return;
    }
#if HAL_HAS_WIRINGPI
    ::pwmWrite(static_cast<int>(pin), static_cast<int>(value));
#else
    (void)value;
#endif
}
} // namespace hal
