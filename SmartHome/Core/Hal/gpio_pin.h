#ifndef GPIO_PIN_H
#define GPIO_PIN_H

#include <cstdint>
#include <expected>
#include <string>

namespace hal {

// Valid GPIO pin number.
class GpioPin {
  public:
    static constexpr int COUNT = 31;

    static std::expected<GpioPin, std::string> create(uint8_t number) {
        if (number >= COUNT) {
            return std::unexpected("Invalid GPIO pin: expected an integer from 0 to 30");
        }
        return GpioPin(number);
    }

    uint8_t number() const {
        return m_Number;
    }

  private:
    explicit GpioPin(uint8_t number) : m_Number(number) {}

    uint8_t m_Number;
};

enum class PinMode : uint8_t {
    INPUT = 0u,
    OUTPUT = 1u,
    PWM_OUTPUT = 2u,
    // GPIO_CLOCK = 3u,
};

enum class PinState : uint8_t {
    LOW = 0u,
    HIGH = 1u,
};

} // namespace hal

#endif // GPIO_PIN_H
