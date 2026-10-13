#pragma once

#include <optional>

#include "app_state.h"
#include "config_io.h"
#include "home_ctrl.h"

namespace smart_home::app {

struct LoadedConfig {
    ConfigSnapshot home;
    std::optional<nlohmann::json> pins;
    HomeSettings settings;
};

struct SaveRequest {
    nlohmann::json document;
    std::string version;
    uint64_t revision;
};

// Owns the one live state and device lifetime. All methods except readConfig/writeConfig
// run on the UI thread. Workers receive immutable file-operation snapshots only.
class AppSession {
  public:
    explicit AppSession(std::string configPath);
    AppSession(const AppSession &) = delete;
    AppSession &operator=(const AppSession &) = delete;

    AppState &state() {
        return m_state;
    }
    const AppState &state() const {
        return m_state;
    }
    bool loaded() const {
        return m_home.has_value();
    }
    bool hasUnsavedChanges() const {
        return m_revision != m_savedRevision;
    }
    void edited() {
        ++m_revision;
    }
    const std::string &path() const {
        return m_path;
    }

    static std::expected<LoadedConfig, std::string> readConfig(const std::string &path,
                                                               bool readPins);
    std::expected<void, std::string> acceptLoad(LoadedConfig config);
    std::expected<void, std::string> reload();
    std::expected<SaveRequest, std::string> prepareSave() const;
    static std::expected<ConfigSnapshot, ConfigError> writeConfig(const std::string &path,
                                                                  const SaveRequest &request);
    void acceptSave(ConfigSnapshot config, uint64_t revision);
    std::expected<void, std::string> save();
    void update();
    std::string deviceStatus() const;

  private:
    std::string m_path;
    AppState m_state;
    std::optional<HomeControl> m_home;
    ConfigSnapshot m_accepted;
    uint64_t m_revision = 0;
    uint64_t m_savedRevision = 0;
    std::string m_applyError;
    std::string m_sensorError;
};

} // namespace smart_home::app
