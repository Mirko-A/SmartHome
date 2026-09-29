#include "ac.h"

#include "gpio.h"

Ac::Ac(Pins pins) : m_Pins(std::move(pins)), m_On(false), m_Mode(Ac::Mode::NORMAL), m_Speed(0) {}

std::expected<Ac, std::string> Ac::create(hal::GpioPin pin1, hal::GpioPin pin2) {
    auto gpioResult = hal::Gpio::instance();
    if (!gpioResult) {
        return std::unexpected(gpioResult.error());
    }
    hal::Gpio &gpio = gpioResult->get();

    auto result1 = gpio.take(pin1, hal::PinMode::PWM_OUTPUT);
    if (!result1) {
        return std::unexpected(result1.error());
    }
    auto result2 = gpio.take(pin2, hal::PinMode::PWM_OUTPUT);
    if (!result2) {
        return std::unexpected(result2.error());
    }

    return Ac(Pins{std::move(*result1), std::move(*result2)});
}

void Ac::setOn(bool on) {
    m_On = on;
}

void Ac::setMode(Ac::Mode mode) {
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
        case Ac::Mode::NORMAL: {
            // Run at m_Speed
        } break;
        case Ac::Mode::FAST: {
            // Run at 1.5 * m_Speed
        } break;
        case Ac::Mode::TURBO: {
            // Run at 2 * m_Speed
        } break;
        default: {
        } break;
        }
    } else {
        // Stop AC
    }
}
