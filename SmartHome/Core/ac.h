#ifndef AC_H
#define AC_H

#include <cstdint>
#include <expected>
#include <string>

#include "hal.h"
#include "smart_home_types.h"

class Ac {
  public:
    struct Pins {
        // FIXME: Change to actual pins.
        Pins(GpioPin pin1 = GpioPin::NONE, GpioPin pin2 = GpioPin::NONE) {
            this->pin1 = pin1;
            this->pin2 = pin2;
        };

        GpioPin pin1;
        GpioPin pin2;
    };

  public:
    Ac();

    Ac(const Ac &) = delete;
    Ac(Ac &&) = delete;

    std::expected<void, std::string> initPins(GpioPin pin1, GpioPin pin2);

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
