#ifndef AC_H
#define AC_H

#include <cstdint>
#include <expected>
#include <string>

#include "owned_pin.h"

class Ac {
  public:
    enum class Mode {
        NORMAL,
        FAST,
        TURBO,
    };

    struct Pins {
        hal::OwnedPin pin1;
        hal::OwnedPin pin2;
    };

  public:
    static std::expected<Ac, std::string> create(hal::GpioPin pin1, hal::GpioPin pin2);

    Ac(const Ac &) = delete;
    Ac &operator=(const Ac &) = delete;
    Ac(Ac &&) = default;
    Ac &operator=(Ac &&) = default;

    void setOn(bool on);
    void setMode(Ac::Mode mode);
    void setSpeed(uint8_t speed);

    std::string modeAsString() const {
        switch (m_Mode) {
        case Ac::Mode::NORMAL:
            return "Normal";
        case Ac::Mode::FAST:
            return "Fast";
        case Ac::Mode::TURBO:
            return "Turbo";
        }
    }

    void Run();

  private:
    explicit Ac(Pins pins);

  private:
    Pins m_Pins;
    bool m_On;
    Ac::Mode m_Mode;
    uint8_t m_Speed;
};

#endif // AC_H
