#ifndef LIGHT_H
#define LIGHT_H

#include <expected>
#include <string>

#include "gpio.h"
#include "smart_home_types.h"

class Light {
  public:
    struct Pins {
        Pins(hal::GpioPin livingRoom = hal::GpioPin::NONE, hal::GpioPin bedroom = hal::GpioPin::NONE,
             hal::GpioPin kitchen = hal::GpioPin::NONE) {
            this->livingRoom = livingRoom;
            this->bedroom = bedroom;
            this->kitchen = kitchen;
        };

        hal::GpioPin livingRoom;
        hal::GpioPin bedroom;
        hal::GpioPin kitchen;
    };

  public:
    Light();

    Light(const Light &) = delete;
    Light(Light &&) = delete;

    std::expected<void, std::string> initPins(hal::GpioPin livingRoomPin, hal::GpioPin bedRoomPin,
                                              hal::GpioPin kitchenPin);

    std::expected<void, std::string> setOn(bool on, LightLocation location);

  private:
    Pins m_Pins;
};

#endif // LIGHT_H
