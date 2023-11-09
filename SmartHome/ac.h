#ifndef AC_H
#define AC_H

#include "smart_home_types.h"

#include <cstdint>

class AC
{
public:
    struct Pins
    {
        // TODO: add pins
        Pins(uint8_t pin1 = 0, uint8_t pin2 = 0)
        {
            this->pin1 = pin1;
            this->pin2 = pin2;
        };

        Pins(const Pins& other)
        {
            pin1 = other.pin1;
            pin2 = other.pin2;
        };

        uint8_t pin1;
        uint8_t pin2;
    };

public:
    AC(const Pins& pins);
    ~AC();

    void setOn(bool on);
    void setMode(ACMode mode);
    void setSpeed(uint8_t speed);

    void Run();

private:
    Pins    m_Pins;
    bool    m_On;
    ACMode  m_Mode;
    uint8_t m_Speed;
};

#endif // AC_H
