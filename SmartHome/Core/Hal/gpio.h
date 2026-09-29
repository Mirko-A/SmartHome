#ifndef GPIO_H
#define GPIO_H

#include <array>
#include <expected>
#include <functional>
#include <optional>
#include <string>

#include "owned_pin.h"

namespace hal {

// All HAL and device operations, including destruction, must use one thread.
class Gpio {
  public:
    static std::expected<std::reference_wrapper<Gpio>, std::string> instance() {
        static Gpio gpio;
        if (gpio.m_Initialized) {
            return std::ref(gpio);
        } else {
            return std::unexpected("gpio initialization failed");
        }
    }

    Gpio(const Gpio &) = delete;
    Gpio &operator=(const Gpio &) = delete;

    // Outputs start and finish at inactiveState; PWM starts and finishes at zero.
    // Released outputs retain their inactive drive level, rather than floating.
    std::expected<OwnedPin, std::string> take(GpioPin pin, PinMode mode, PinState inactiveState = PinState::LOW);

  private:
    friend class OwnedPin;
    void release(GpioPin pin) noexcept;

    Gpio();
    ~Gpio();

    // Present means available; an empty slot belongs to an active OwnedPin.
    std::array<std::optional<GpioPin>, GpioPin::COUNT> m_AvailablePins;
    bool m_Initialized;
};

} // namespace hal

#endif // GPIO_H
