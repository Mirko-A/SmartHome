#include "ac.h"

Ac::Ac(hal::GpioPin pin1, hal::GpioPin pin2)
    : m_Pins(pin1, pin2), m_On(false), m_Mode(AcMode::NORMAL), m_Speed(0) {}

std::expected<Ac, std::string> Ac::create(hal::GpioPin pin1, hal::GpioPin pin2) {
    auto gpioResult = hal::Gpio::instance();
    if (!gpioResult) {
        return std::unexpected(gpioResult.error());
    }
    hal::Gpio &gpio = gpioResult->get();

    if (auto r = gpio.setPinMode(pin1, hal::PinMode::PWM_OUTPUT); !r) {
        return std::unexpected(r.error());
    }
    if (auto r = gpio.setPinMode(pin2, hal::PinMode::PWM_OUTPUT); !r) {
        return std::unexpected(r.error());
    }

    return Ac(pin1, pin2);
}

void Ac::setOn(bool on) {
    m_On = on;
}

void Ac::setMode(AcMode mode) {
    m_Mode = mode;
}

void Ac::setSpeed(uint8_t speed) {
    m_Speed = speed;
}

void Ac::Run() {
    // TODO: run the AC based on speed
    if (m_On) {
        // Start AC
        switch (m_Mode) {
        case AcMode::NORMAL: {
            // Run at m_Speed
        } break;
        case AcMode::FAST: {
            // Run at 1.5 * m_Speed
        } break;
        case AcMode::TURBO: {
            // Run at 2 * m_Speed
        } break;
        default: {
        } break;
        }
    } else {
        // Stop AC
    }
}
