#ifndef LIGHT_H
#define LIGHT_H

#include <cstdint>
#include <expected>
#include <string>

#include "owned_pin.h"

class Light {
  public:
    enum class Location : uint8_t {
        LIVING_ROOM = 0u,
        BEDROOM,
        KITCHEN,
    };

    struct Pins {
        hal::OwnedPin livingRoom;
        hal::OwnedPin bedroom;
        hal::OwnedPin kitchen;
    };

  public:
    static std::expected<Light, std::string> create(hal::GpioPin livingRoomPin, hal::GpioPin bedRoomPin,
                                                    hal::GpioPin kitchenPin);

    ~Light() = default;

    Light(const Light &) = delete;
    Light &operator=(const Light &) = delete;
    Light(Light &&) = default;
    Light &operator=(Light &&) = default;

    std::expected<void, std::string> setOn(bool on, Light::Location location);

  private:
    explicit Light(Pins pins);

    Pins m_Pins;
};

#endif // LIGHT_H
