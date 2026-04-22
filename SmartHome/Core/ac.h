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
        Pins(hal::GpioPin pin1, hal::GpioPin pin2) {
            this->pin1 = pin1;
            this->pin2 = pin2;
        };

        hal::GpioPin pin1;
        hal::GpioPin pin2;
    };

  public:
    static std::expected<Ac, std::string> create(hal::GpioPin pin1, hal::GpioPin pin2);

    Ac(const Ac &) = delete;
    Ac &operator=(const Ac &) = delete;
    Ac(Ac &&) = default;
    Ac &operator=(Ac &&) = default;

    void setOn(bool on);
    void setMode(AcMode mode);
    void setSpeed(uint8_t speed);

    void Run();

  private:
    Ac(hal::GpioPin pin1, hal::GpioPin pin2);

    Pins m_Pins;
    bool m_On;
    AcMode m_Mode;
    uint8_t m_Speed;
};

#endif // AC_H
