#include "ac.h"

Ac::Ac() : m_On(false), m_Mode(AcMode::NORMAL), m_Speed(0) {}

std::expected<void, std::string> Ac::initPins(GpioPin pin1, GpioPin pin2) {
    m_Pins.pin1 = pin1;
    m_Pins.pin2 = pin2;

    if (m_Pins.pin1 == GpioPin::NONE || m_Pins.pin2 == GpioPin::NONE) {
        return std::unexpected("Invalid GPIO pin(s) for AC");
    }
    // Only GPIO pins that support PWM output (12 and 18) can be used for AC control.
    switch (m_Pins.pin1) {
    case GpioPin::GPIO_12:
    case GpioPin::GPIO_18:
        break;
    default:
        return std::unexpected("Invalid GPIO pin 1 for AC");
    }
    switch (m_Pins.pin2) {
    case GpioPin::GPIO_12:
    case GpioPin::GPIO_18:
        break;
    default:
        return std::unexpected("Invalid GPIO pin 2 for AC");
    }

    Hal &hal = Hal::instance();
    if (!hal.isInitialized()) {
        return std::unexpected("HAL initialization failed");
    }

    hal.pinMode(m_Pins.pin1, PinMode::PWM_OUTPUT);
    hal.pinMode(m_Pins.pin2, PinMode::PWM_OUTPUT);
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
