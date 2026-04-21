#ifndef LIGHT_H
#define LIGHT_H

#include "hal.h"
#include "smart_home_types.h"

class Light {
  public:
    struct Pins {
        Pins(GpioPin livingRoom = GpioPin::NONE, GpioPin bedroom = GpioPin::NONE,
             GpioPin kitchen = GpioPin::NONE) {
            this->livingRoom = livingRoom;
            this->bedroom = bedroom;
            this->kitchen = kitchen;
        };

        GpioPin livingRoom;
        GpioPin bedroom;
        GpioPin kitchen;
    };

  public:
    Light();

    Light(const Light &) = delete;
    Light(Light &&) = delete;

    void initPins(GpioPin livingRoomPin, GpioPin bedRoomPin, GpioPin kitchenPin);

    void setOn(bool on, LightLocation location);

  private:
    Pins m_Pins;
};

#endif // LIGHT_H
