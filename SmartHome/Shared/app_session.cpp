#include "app_session.h"

#include <cmath>
#include <filesystem>
#include <limits>

namespace smart_home::app {

AppSession::AppSession(std::string configPath) : m_path(std::move(configPath)) {}

std::expected<LoadedConfig, std::string> AppSession::readConfig(const std::string &path,
                                                                bool readPins) {
    auto home = loadConfig(path);
    if (!home) {
        return std::unexpected(home.error().message);
    }
    HomeSettings settings;
    if (auto valid = settings.deserializeJson(home->document); !valid) {
        return std::unexpected("Invalid settings (" + path + "): " + valid.error());
    }
    std::optional<nlohmann::json> pins;
    if (readPins) {
        auto result =
            loadConfig((std::filesystem::path(path).parent_path() / "pin_cfg.json").string());
        if (!result) {
            return std::unexpected(result.error().message);
        }
        pins = std::move(result->document);
    }
    return LoadedConfig{std::move(*home), std::move(pins), settings};
}

std::expected<void, std::string> AppSession::acceptLoad(LoadedConfig config) {
    if (!m_home) {
        if (!config.pins) {
            return std::unexpected("Pin configuration is missing");
        }
        auto home = HomeControl::create(*config.pins, config.home.document);
        if (!home) {
            return std::unexpected(home.error());
        }
        m_home.emplace(std::move(*home));
    }
    m_state = AppState(config.settings);
    m_accepted = std::move(config.home);
    m_savedRevision = m_revision++;
    // Loading succeeded even when application/acquisition fails; report those separately.
    update();
    return {};
}

std::expected<void, std::string> AppSession::reload() {
    auto config = readConfig(m_path, !loaded());
    if (!config) {
        return std::unexpected(config.error());
    }
    return acceptLoad(std::move(*config));
}

std::expected<SaveRequest, std::string> AppSession::prepareSave() const {
    if (!loaded()) {
        return std::unexpected("No valid configuration loaded");
    }
    auto settings = m_state.snapshot();
    if (!settings) {
        return std::unexpected(settings.error());
    }
    auto document = m_accepted.document;
    document.merge_patch(settings->serializeJson());
    return SaveRequest{std::move(document), m_accepted.version, m_revision};
}

std::expected<ConfigSnapshot, ConfigError> AppSession::writeConfig(const std::string &path,
                                                                   const SaveRequest &request) {
    return saveConfig(path, request.document, request.version);
}

void AppSession::acceptSave(ConfigSnapshot config, uint64_t revision) {
    m_accepted = std::move(config);
    m_savedRevision = revision;
}

std::expected<void, std::string> AppSession::save() {
    auto request = prepareSave();
    if (!request) {
        return std::unexpected(request.error());
    }
    auto result = writeConfig(m_path, *request);
    if (!result) {
        return std::unexpected(result.error().message);
    }
    acceptSave(std::move(*result), request->revision);
    return {};
}

void AppSession::update() {
    if (!m_home) {
        return;
    }
    auto settings = m_state.snapshot();
    if (!settings) {
        m_applyError = settings.error();
    } else {
        auto result = m_home->onUpdate(*settings);
        m_applyError = result ? "" : result.error();
    }
    m_sensorError.clear();
    const auto read = [&](Sensor::Type type, int16_t &value) {
        auto result = m_home->readSensor(type);
        bool valid = result && std::isfinite(*result) &&
                     *result >= std::numeric_limits<int16_t>::min() &&
                     *result <= std::numeric_limits<int16_t>::max();
        if (valid) {
            value = static_cast<int16_t>(*result);
        } else {
            if (!m_sensorError.empty()) {
                m_sensorError += "; ";
            }
            m_sensorError += result ? "Sensor reading is out of range" : result.error();
        }
    };
    read(Sensor::Type::TEMPERATURE, m_state.sensors.temperature);
    read(Sensor::Type::HUMIDITY, m_state.sensors.humidity);
    read(Sensor::Type::BRIGHTNESS, m_state.sensors.brightness);
}

// TODO: Debug only part of the status. Remove once support is added for all devices.
std::string AppSession::deviceStatus() const {
    if (!loaded()) {
        return "No valid configuration loaded.";
    }
    std::string status = m_applyError.empty() ? "Light writes accepted."
                                              : "Light application failed: " + m_applyError;
    status += " AC and speakers are settings-only (unsupported).";
    if (!m_sensorError.empty()) {
        status += " " + m_sensorError;
    }
    return status;
}

} // namespace smart_home::app
