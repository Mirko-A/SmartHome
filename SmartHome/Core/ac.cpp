#include "ac.h"

Ac::Ac() : m_On(false), m_Mode(AcMode::NORMAL), m_Speed(0) {}

void Ac::initPins(uint8_t pin1, uint8_t pin2) {
    m_Pins.pin1 = pin1;
    m_Pins.pin2 = pin2;

    // TODO: Init pins with HAL
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
