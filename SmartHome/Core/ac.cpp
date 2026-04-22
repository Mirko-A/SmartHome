#include "ac.h"

Ac::Ac() : m_On(false), m_Mode(AcMode::NORMAL), m_Speed(0) {}

std::expected<void, std::string> Ac::initPins(hal::GpioPin pin1, hal::GpioPin pin2) {
    m_Pins.pin1 = pin1;
    m_Pins.pin2 = pin2;

    if (m_Pins.pin1 == hal::GpioPin::NONE || m_Pins.pin2 == hal::GpioPin::NONE) {
        return std::unexpected("Invalid GPIO pin(s) for AC");
    }
    // Only GPIO pins that support PWM output (12 and 18) can be used for AC control.
    switch (m_Pins.pin1) {
    case hal::GpioPin::GPIO_12:
    case hal::GpioPin::GPIO_18:
        break;
    default:
        return std::unexpected("Invalid GPIO pin 1 for AC");
    }
    switch (m_Pins.pin2) {
    case hal::GpioPin::GPIO_12:
    case hal::GpioPin::GPIO_18:
        break;
    default:
        return std::unexpected("Invalid GPIO pin 2 for AC");
    }

    hal::Gpio &gpio = hal::Gpio::instance();
    if (!gpio.isInitialized()) {
        return std::unexpected("gpio initialization failed");
    }

    gpio.pinMode(m_Pins.pin1, hal::PinMode::PWM_OUTPUT);
    gpio.pinMode(m_Pins.pin2, hal::PinMode::PWM_OUTPUT);
    return std::expected<void, std::string>();
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
