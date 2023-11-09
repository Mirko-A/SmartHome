#ifndef LIGHT_H
#define LIGHT_H

#include "smart_home_types.h"

#include <cstdint>

class Light
{
public:
    struct Pins
    {
        // TODO: add pins
        Pins(uint8_t livingRoom = 0U, uint8_t bedRoom = 0U, uint8_t kitchen = 0U)
        {
            this->livingRoom = livingRoom;
            this->bedRoom    = bedRoom;
            this->kitchen    = kitchen;
        };

        Pins(const Pins& other)
        {
            livingRoom = other.livingRoom;
            bedRoom    = other.bedRoom;
            kitchen    = other.kitchen;
        };

        uint8_t livingRoom;
        uint8_t bedRoom;
        uint8_t kitchen;
    };

public:
    Light(const Pins& pins);

    void setOn(bool on, LightLocation location);

private:
    Pins m_Pins;
};

#endif // LIGHT_H
