#include "home_config.h"
#include <cmath>

HomeConfig::HomeConfig()
    : m_LightSettings(LightSettings()), m_ACSettings(ACSettings()), m_SensorReadings(SensorReadings()), m_SpeakerSettings(SpeakerSettings()),
      m_Light(Light::Pins()), m_AC(AC::Pins()), m_Sensor(Sensor::Pins()),
      m_Dirty(false)
{

}

void HomeConfig::readHardwareInputs()
{
    // Sensors (this should make the cfg dirty)
    m_SensorReadings.temperature = static_cast<int16_t>(m_Sensor.read(Sensor::Type::TEMPERATURE));
    m_SensorReadings.humidity    = static_cast<int16_t>(m_Sensor.read(Sensor::Type::HUMIDITY));
    m_SensorReadings.brightness  = static_cast<int16_t>(m_Sensor.read(Sensor::Type::BRIGHTNESS));
    m_Dirty = true;
}

void HomeConfig::sendHardwareOutputs()
{
    // Lights
    m_Light.setOn(m_LightSettings.livingRoomLightOn, LightLocation::LIVING_ROOM);
    m_Light.setOn(m_LightSettings.bedroomLightOn, LightLocation::BEDROOM);
    m_Light.setOn(m_LightSettings.kitchenLightOn, LightLocation::KITCHEN);
    // AC
    m_AC.setOn(m_ACSettings.on);
    m_AC.setMode(m_ACSettings.mode);
    // TODO: Mess with speed until right
    uint8_t ACSpeed = 10 * abs(m_ACSettings.temperature - m_SensorReadings.temperature);
    m_AC.setSpeed(ACSpeed);
    m_AC.Run();
}

void HomeConfig::onUpdate()
{
    // TODO: continue this
    readHardwareInputs();
    sendHardwareOutputs();
}

void HomeConfig::fromJSON(const nlohmann::json& thisAsJson)
{
    m_LightSettings.fromJSON(thisAsJson["Lights"]);
    m_SensorReadings.fromJSON(thisAsJson["SensorReadings"]);
    m_ACSettings.fromJSON(thisAsJson["AC"]);
    m_SpeakerSettings.fromJSON(thisAsJson["Speakers"]);
}

void HomeConfig::loadDirtyFlag(const nlohmann::json& thisAsJson)
{
    m_Dirty = thisAsJson["Dirty"];
}

nlohmann::json HomeConfig::toJSON()
{
    nlohmann::json thisAsJSON =
    {
        m_LightSettings.toJSON(),
        m_SensorReadings.toJSON(),
        m_ACSettings.toJSON(),
        m_SpeakerSettings.toJSON(),
        { "Dirty", m_Dirty },
    };

    return thisAsJSON;
}

QString ACModeToString(ACMode mode)
{
    QString modeStr;

    switch (mode)
    {
        case ACMode::NORMAL:
            modeStr = "Normal";
        break;
        case ACMode::FAST:
            modeStr = "Fast";
        break;
        case ACMode::TURBO:
            modeStr = "Turbo";
        break;
        default:
            modeStr = "Error";
        break;
    }

    return modeStr;
}
