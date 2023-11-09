#ifndef HAL_H
#define HAL_H

#include <cstdint>

// TODO: HAL not done yet, still need to implement all functions
class HAL
{
public:
    HAL();
    ~HAL();

    uint8_t digitalRead(uint8_t pin);
    void digitalWrite(uint8_t value, uint8_t pin);

    void createPWM(uint8_t pin);
    void sendPWM(uint8_t value, uint8_t pin);

private:
    bool m_InitOK;
};


#endif // HAL_H
