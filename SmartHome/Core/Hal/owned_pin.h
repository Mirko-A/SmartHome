#ifndef OWNED_PIN_H
#define OWNED_PIN_H

#include "gpio_pin.h"

namespace hal {

class Gpio;

// Exclusive, movable access to one GPIO pin. Destruction transfers ownership
// back to the HAL.
//
// The HAL must outlive this handle. All HAL and device operations must use one
// thread.
class OwnedPin {
  public:
    ~OwnedPin();
    OwnedPin(const OwnedPin &) = delete;
    OwnedPin &operator=(const OwnedPin &) = delete;
    OwnedPin(OwnedPin &&other) noexcept;
    OwnedPin &operator=(OwnedPin &&other) noexcept;

    // Idempotent: deactivate and return the pin. Moved-from handles do nothing.
    //
    // NOTE: WiringPi writes provide no failure status; this is best-effort cleanup.
    void shutdown() noexcept;

    std::expected<PinState, std::string> digitalRead();
    std::expected<void, std::string> digitalWrite(PinState state);
    std::expected<int, std::string> analogRead();
    std::expected<void, std::string> pwmWrite(uint16_t value);

  private:
    friend class Gpio;

    OwnedPin(Gpio &gpio, GpioPin pin, PinMode mode, PinState inactiveState);

    Gpio *m_Gpio;
    GpioPin m_Pin;
    PinMode m_Mode;
    PinState m_InactiveState;
};

} // namespace hal

#endif // OWNED_PIN_H
