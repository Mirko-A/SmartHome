#ifndef HOME_CTRL_H
#define HOME_CTRL_H

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

    std::expected<void, std::string> onUpdate();

    void loadDirtyFlag(const nlohmann::json &thisAsJson);
    std::expected<void, std::string> deserializeJson(const nlohmann::json &json);
    nlohmann::json serializeJson() const;

    HomeSettings &settings() {
        return m_Settings;
    }
    const HomeSettings &settings() const {
        return m_Settings;
    }

  private:
    HomeControl() = delete;
    HomeControl(Light light, Ac ac, Sensor sensor);

  private:
    HomeSettings m_Settings;
    Light m_Light;
    Ac m_Ac;
    Sensor m_Sensor;

  public:
    bool m_Dirty;
};

#endif // HOME_CTRL_H
