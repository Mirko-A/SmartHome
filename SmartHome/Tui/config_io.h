#pragma once

#include <expected>
#include <nlohmann/json.hpp>
#include <string>

namespace smart_home::tui {

// Missing files and read or parse failures are reported to the caller.
std::expected<nlohmann::json, std::string> loadConfig(const std::string &path);
std::expected<void, std::string> saveConfig(const std::string &path, const nlohmann::json &config);

} // namespace smart_home::tui
