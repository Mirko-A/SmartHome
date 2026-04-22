#ifndef LIGHT_H
#define LIGHT_H

#include <expected>
#include <string>

#include "gpio.h"
#include "smart_home_types.h"

class Light {
  public:
    struct Pins {
        Pins(hal::GpioPin livingRoom, hal::GpioPin bedroom, hal::GpioPin kitchen) {
            this->livingRoom = livingRoom;
            this->bedroom = bedroom;
            this->kitchen = kitchen;
        };

        hal::GpioPin livingRoom;
        hal::GpioPin bedroom;
        hal::GpioPin kitchen;
    };

  public:
    static std::expected<Light, std::string> create(hal::GpioPin livingRoomPin, hal::GpioPin bedRoomPin,
                                                    hal::GpioPin kitchenPin);

    Light(const Light &) = delete;
    Light &operator=(const Light &) = delete;
    Light(Light &&) = default;
    Light &operator=(Light &&) = default;

    std::expected<void, std::string> setOn(bool on, LightLocation location);

  private:
    Light(hal::GpioPin livingRoom, hal::GpioPin bedroom, hal::GpioPin kitchen);

    Pins m_Pins;
};

#endif // LIGHT_H
