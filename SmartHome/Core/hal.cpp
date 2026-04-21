#include "hal.h"

#if __has_include(<wiringPi.h>)
#include <wiringPi.h>
#define Hal_HAS_WIRINGPI 1
#else
#define Hal_HAS_WIRINGPI 0
static int wiringPiSetup() {
    return 0;
}
#endif

Hal::Hal() {
    m_Initialized = (wiringPiSetup() >= 0);
}

Hal::~Hal() = default;

void Hal::pinMode(GpioPin pin, PinMode mode) {
    if (pin == GpioPin::NONE) {
        return;
    }
#if Hal_HAS_WIRINGPI
    ::pinMode(static_cast<int>(pin), static_cast<int>(mode));
#else
    (void)mode;
#endif
}

PinState Hal::digitalRead(GpioPin pin) {
    if (pin == GpioPin::NONE) {
        return PinState::LOW;
    }
#if Hal_HAS_WIRINGPI
    return (::digitalRead(static_cast<int>(pin)) != 0) ? PinState::HIGH : PinState::LOW;
#else
    return PinState::LOW;
#endif
}

void Hal::digitalWrite(GpioPin pin, PinState state) {
    if (pin == GpioPin::NONE) {
        return;
    }
#if Hal_HAS_WIRINGPI
    ::digitalWrite(static_cast<int>(pin), static_cast<int>(state));
#else
    (void)state;
#endif
}

int Hal::analogRead(GpioPin pin) {
    if (pin == GpioPin::NONE) {
        return 0;
    }
#if Hal_HAS_WIRINGPI
    return ::analogRead(static_cast<int>(pin));
#else
    return 0;
#endif
}

void Hal::pwmWrite(GpioPin pin, uint16_t value) {
    if (pin == GpioPin::NONE) {
        return;
    }
#if Hal_HAS_WIRINGPI
    ::pwmWrite(static_cast<int>(pin), static_cast<int>(value));
#else
    (void)value;
#endif
}

bool Hal::readDHT22(GpioPin pin, float &temp, float &humidity) {
    (void)pin;
    (void)temp;
    (void)humidity;
    return false;
}
