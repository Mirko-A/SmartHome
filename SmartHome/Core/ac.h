#ifndef AC_H
#define AC_H

#include <cstdint>
#include <expected>
#include <string>

#include "gpio.h"
#include "smart_home_types.h"

class Ac {
  public:
    struct Pins {
        // FIXME: Change to actual pins.
        Pins(hal::GpioPin pin1 = hal::GpioPin::NONE, hal::GpioPin pin2 = hal::GpioPin::NONE) {
            this->pin1 = pin1;
            this->pin2 = pin2;
        };

        hal::GpioPin pin1;
        hal::GpioPin pin2;
    };

  public:
    Ac();

    Ac(const Ac &) = delete;
    Ac(Ac &&) = delete;

    std::expected<void, std::string> initPins(hal::GpioPin pin1, hal::GpioPin pin2);

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
