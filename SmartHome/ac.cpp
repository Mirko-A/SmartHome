#include "ac.h"

AC::AC(const Pins& pins)
    : m_Pins(pins), m_On(false), m_Mode(ACMode::NORMAL), m_Speed(0)
{
    // TODO: Init pins with HAL
}

void AC::setOn(bool on)
{
    m_On = on;
}

void AC::setMode(ACMode mode)
{
    m_Mode = mode;
}

void AC::setSpeed(uint8_t speed)
{
    m_Speed = speed;
}

void AC::Run()
{
    // TODO: run the AC based on speed
    if(m_On)
    {
        // Start AC
        switch(m_Mode)
        {
            case ACMode::NORMAL:
            {
                // Run at m_Speed
            }
            break;
            case ACMode::FAST:
            {
                // Run at 1.5 * m_Speed
            }
            break;
            case ACMode::TURBO:
            {
                // Run at 2 * m_Speed
            }
            break;
            default:
            {

            }
            break;
        }
    }
    else
    {
        // Stop AC
    }
}
