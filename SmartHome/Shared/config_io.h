#pragma once

#include <expected>
#include <nlohmann/json.hpp>
#include <string>

namespace smart_home::app {

enum class ConfigErrorKind { MISSING, IO, PARSE, VALIDATION, CONFLICT };
struct ConfigError {
    ConfigErrorKind kind;
    std::string message;
};
struct ConfigSnapshot {
    nlohmann::json document;
    // Exact bytes, not a timestamp: even a same-size edit must invalidate a save.
    std::string version;
};

// Readers and writers use a persistent <canonical path>.lock sidecar. Lock contention
// is reported immediately. All writers must cooperate; arbitrary editors can bypass it.
std::expected<ConfigSnapshot, ConfigError> loadConfig(const std::string &path);
std::expected<ConfigSnapshot, ConfigError> saveConfig(const std::string &path,
                                                      const nlohmann::json &config,
                                                      const std::string &expectedVersion);

} // namespace smart_home::app
