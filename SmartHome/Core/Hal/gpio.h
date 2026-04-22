#ifndef GPIO_H
#define GPIO_H

#include <array>
#include <cstdint>
#include <expected>
#include <functional>
#include <optional>
#include <string>

namespace hal {

enum class GpioPin : uint8_t {
    GPIO_0 = 0u,
    GPIO_1,
    GPIO_2,
    GPIO_3,
    GPIO_4,
    GPIO_5,
    GPIO_6,
    GPIO_7,
    GPIO_8,
    GPIO_9,
    GPIO_10,
    GPIO_11,
    GPIO_12,
    GPIO_13,
    GPIO_14,
    GPIO_15,
    GPIO_16,
    GPIO_17,
    GPIO_18,
    GPIO_19,
    GPIO_20,
    GPIO_21,
    GPIO_22,
    GPIO_23,
    GPIO_24,
    GPIO_25,
    GPIO_26,
    GPIO_27,
    GPIO_28,
    GPIO_29,
    GPIO_30,
    COUNT,
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

    std::array<std::optional<PinMode>, static_cast<std::size_t>(GpioPin::COUNT)> m_PinModes;
    bool m_Initialized;
};

} // namespace hal

#endif // GPIO_H
