#ifndef AC_H
#define AC_H

#include <cstdint>

#include "smart_home_types.h"

class Ac {
  public:
    struct Pins {
        // TODO: add pins
        Pins(uint8_t pin1 = 0, uint8_t pin2 = 0) {
            this->pin1 = pin1;
            this->pin2 = pin2;
        };

        uint8_t pin1;
        uint8_t pin2;
    };

  public:
    Ac();

    Ac(const Ac &) = delete;
    Ac(Ac &&) = delete;

    void initPins(uint8_t pin1, uint8_t pin2);

    void setOn(bool on);
    void setMode(AcMode mode);
    void setSpeed(uint8_t speed);

    void Run();

  private:
    Pins m_Pins;
    bool m_On;
    AcMode m_Mode;
    uint8_t m_Speed;
};

#endif // AC_H
