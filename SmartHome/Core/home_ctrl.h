#pragma once

#include <stdint.h>

#include <expected>
#include <string>

#include "ac.h"
#include "home_settings.h"
#include "light.h"
#include "nlohmann/json_fwd.hpp"
#include "sensor.h"

constexpr int MAX_AC_TEMP = 30;
constexpr int MIN_AC_TEMP = 15;
constexpr int AC_TEMP_STEP = 1;

class HomeControl {
  public:
    // Validate both configurations before configuring any hardware.
    static std::expected<HomeControl, std::string> create(const nlohmann::json &pinCfgJson,
                                                          const nlohmann::json &homeCfgJson);

    HomeControl(const HomeControl &) = delete;
    HomeControl &operator=(const HomeControl &) = delete;
    HomeControl(HomeControl &&) = default;
    HomeControl &operator=(HomeControl &&) = default;

    // Reapply every supported output; success does not imply physical readback.
    std::expected<void, std::string> onUpdate(const HomeSettings &settings);
    std::expected<float, std::string> readSensor(Sensor::Type type);

  private:
    HomeControl() = delete;
    HomeControl(Light light, Ac ac, Sensor sensor);

  private:
    Light m_Light;
    Ac m_Ac;
    Sensor m_Sensor;
};
