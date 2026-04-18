#ifndef LIGHT_H
#define LIGHT_H

#include <cstdint>

#include "smart_home_types.h"

class Light {
  public:
    struct Pins {
        Pins(uint8_t livingRoom = 0U, uint8_t bedRoom = 0U, uint8_t kitchen = 0U) {
            this->livingRoom = livingRoom;
            this->bedRoom = bedRoom;
            this->kitchen = kitchen;
        };

        uint8_t livingRoom;
        uint8_t bedRoom;
        uint8_t kitchen;
    };

  public:
    Light(uint8_t livingRoomPin, uint8_t bedRoomPin, uint8_t kitchenPin);

    Light() = delete;
    Light(const Light &) = delete;
    Light(Light &&) = delete;

    void setOn(bool on, LightLocation location);

  private:
    Pins m_Pins;
};

#endif // LIGHT_H
