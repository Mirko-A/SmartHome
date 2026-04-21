#ifndef HAL_H
#define HAL_H

#include <cstdint>

enum class GpioPin : uint8_t {
    NONE = 0xFFu,
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

class Hal {
  public:
    static Hal &instance() {
        static Hal hal;
        return hal;
    }

    Hal(const Hal &) = delete;
    Hal &operator=(const Hal &) = delete;

    bool isInitialized() const {
        return m_Initialized;
    }

    void pinMode(GpioPin pin, PinMode mode);

    PinState digitalRead(GpioPin pin);
    void digitalWrite(GpioPin pin, PinState state);

    int analogRead(GpioPin pin);

    void pwmWrite(GpioPin pin, uint16_t value);

    bool readDHT22(GpioPin pin, float &temp, float &humidity);

  private:
    Hal();
    ~Hal();

    bool m_Initialized;
};

#endif // HAL_H
