#ifndef GPIO_H
#define GPIO_H

#include <array>
#include <cstdint>
#include <expected>
#include <functional>
#include <optional>
#include <string>

namespace hal {

class GpioPin {
  public:
    static constexpr int COUNT = 31;

    static std::expected<GpioPin, std::string> create(uint8_t value) {
        if (value >= COUNT) {
            return std::unexpected("Invalid GPIO pin: expected an integer from 0 to 30");
        }
        return GpioPin(value);
    }

    uint8_t number() const {
        return m_Value;
    }

  private:
    explicit GpioPin(uint8_t value) : m_Value(value) {}

    uint8_t m_Value;
};

enum class PinMode : uint8_t {
    INPUT = 0u,
    OUTPUT = 1u,
    PWM_OUTPUT = 2u,
    GPIO_CLOCK = 3u,
};

enum class PinState : uint8_t {
    LOW = 0u,
    HIGH = 1u,
};

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

    std::expected<void, std::string> setPinMode(GpioPin pin, PinMode mode);

    std::expected<PinState, std::string> digitalRead(GpioPin pin);
    std::expected<void, std::string> digitalWrite(GpioPin pin, PinState state);

    std::expected<int, std::string> analogRead(GpioPin pin);

    std::expected<void, std::string> pwmWrite(GpioPin pin, uint16_t value);

    bool readDHT22(GpioPin pin, float &temp, float &humidity);

  private:
    Gpio();
    ~Gpio();

    std::array<std::optional<PinMode>, GpioPin::COUNT> m_PinModes;
    bool m_Initialized;
};

} // namespace hal

#endif // GPIO_H
