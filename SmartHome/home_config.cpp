#include "home_config.h"

HomeConfig::HomeConfig()
{
    settings.lights.livingRoomLightOn = false;
    settings.lights.bedroomLightOn = false;
    settings.lights.kitchenLightOn = false;

    settings.sensors.temperature = 0;
    settings.sensors.humidity = 0;
    settings.sensors.brightness = 0;

    settings.AC.ACOn = false;
    settings.AC.temperature = 20;
    settings.AC.mode = ACMode::NORMAL;

    settings.speakers.volume = 0;
    settings.speakers.bass = 0;
    settings.speakers.pitch = 0;
}
