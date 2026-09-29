#include "home_settings.h"

#include <limits>
#include <stdexcept>
#include <utility>

namespace {

int readInteger(const nlohmann::json &json, const char *group, const char *key, int min, int max) {
    const auto &value = json.at(group).at(key);
    bool inRange = false;
    if (value.is_number_unsigned()) {
        const auto number = value.get<uint64_t>();
        inRange = std::cmp_greater_equal(number, min) && std::cmp_less_equal(number, max);
    } else if (value.is_number_integer()) {
        const auto number = value.get<int64_t>();
        inRange = number >= min && number <= max;
    }
    if (!inRange) {
        throw std::invalid_argument(std::string(group) + "." + key + ": expected an integer from " +
                                    std::to_string(min) + " to " + std::to_string(max));
    }
    return value.get<int>();
}

} // namespace

std::expected<void, std::string> HomeSettings::setAc(bool on, Ac::Mode mode) {
    switch (mode) {
    case Ac::Mode::NORMAL:
    case Ac::Mode::FAST:
    case Ac::Mode::TURBO:
        m_Ac = {on, mode};
        return {};
    }
    return std::unexpected("ac.mode: invalid AC mode");
}

std::expected<void, std::string> HomeSettings::setSpeakers(int volume, int bass, int pitch) {
    if (volume < 0 || volume > 100 || bass < 0 || bass > 100 || pitch < 0 || pitch > 100) {
        return std::unexpected("speakers: volume, bass and pitch must be from 0 to 100");
    }
    m_Speakers = {static_cast<int16_t>(volume), static_cast<int16_t>(bass), static_cast<int16_t>(pitch)};
    return {};
}

std::expected<void, std::string> HomeSettings::loadFromJson(const nlohmann::json &json) {
    HomeSettings candidate;
    try {
        const auto &lights = json.at("lights");
        candidate.setLights(
            {lights.at("living_room").get<bool>(), lights.at("bedroom").get<bool>(), lights.at("kitchen").get<bool>()});

        constexpr int minReading = std::numeric_limits<int16_t>::min();
        constexpr int maxReading = std::numeric_limits<int16_t>::max();
        candidate.setSensors({
            static_cast<int16_t>(readInteger(json, "sensors", "temperature", minReading, maxReading)),
            static_cast<int16_t>(readInteger(json, "sensors", "humidity", minReading, maxReading)),
            static_cast<int16_t>(readInteger(json, "sensors", "brightness", minReading, maxReading)),
        });

        auto ac = candidate.setAc(json.at("ac").at("on").get<bool>(),
                                  static_cast<Ac::Mode>(readInteger(json, "ac", "mode", 0, 2)));
        if (!ac) {
            return std::unexpected(ac.error());
        }
        auto speakers = candidate.setSpeakers(readInteger(json, "speakers", "volume", 0, 100),
                                              readInteger(json, "speakers", "bass", 0, 100),
                                              readInteger(json, "speakers", "pitch", 0, 100));
        if (!speakers) {
            return std::unexpected(speakers.error());
        }
    } catch (const nlohmann::json::exception &error) {
        return std::unexpected(std::string("Invalid home settings: ") + error.what());
    } catch (const std::invalid_argument &error) {
        return std::unexpected(error.what());
    }
    // Commit only after every field has been parsed and validated.
    *this = candidate;
    return {};
}

nlohmann::json HomeSettings::toJson() const {
    return {
        {"lights",
         {{"living_room", m_Lights.livingRoomLightOn},
          {"bedroom", m_Lights.bedroomLightOn},
          {"kitchen", m_Lights.kitchenLightOn}}},
        {"sensors",
         {{"temperature", m_Sensors.temperature},
          {"humidity", m_Sensors.humidity},
          {"brightness", m_Sensors.brightness}}},
        {"ac", {{"on", m_Ac.on}, {"mode", m_Ac.mode}}},
        {"speakers", {{"volume", m_Speakers.volume}, {"bass", m_Speakers.bass}, {"pitch", m_Speakers.pitch}}},
    };
}
