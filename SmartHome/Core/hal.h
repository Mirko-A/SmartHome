#ifndef HAL_H
#define HAL_H

#include <cstdint>

class HAL {
  public:
    static HAL &instance() {
        static HAL hal;
        return hal;
    }

    HAL(const HAL &) = delete;
    HAL &operator=(const HAL &) = delete;

    void pinMode(uint8_t pin, uint8_t mode);

    uint8_t digitalRead(uint8_t pin);
    void digitalWrite(uint8_t pin, uint8_t value);

    int analogRead(uint8_t pin);

    void pwmWrite(uint8_t pin, uint16_t value);

    bool readDHT22(uint8_t pin, float &temp, float &humidity);

  private:
    HAL();
    ~HAL();

    bool m_Initialized;
};

#endif // HAL_H
