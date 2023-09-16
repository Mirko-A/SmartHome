#include "home_config.h"

void HomeConfig::readHardwareInputs()
{
    // Sensors (this should make the cfg dirty)
}

void HomeConfig::sendHardwareOutputs()
{
    // Lights
    // AC
}

void HomeConfig::onUpdate()
{
    // TODO: continue this
    readHardwareInputs();
    sendHardwareOutputs();
}

void HomeConfig::fromJSON(nlohmann::json thisAsJson)
{
    lights.fromJSON(thisAsJson["Lights"]);
    sensors.fromJSON(thisAsJson["Sensors"]);
    AC.fromJSON(thisAsJson["AC"]);
    speakers.fromJSON(thisAsJson["Speakers"]);
    isDirty = thisAsJson["Dirty"];
}

nlohmann::json HomeConfig::toJSON()
{
    nlohmann::json thisAsJSON =
    {
        lights.toJSON(),
        sensors.toJSON(),
        AC.toJSON(),
        speakers.toJSON(),
        { "Dirty", isDirty },
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
