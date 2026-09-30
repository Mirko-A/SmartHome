#pragma once

#include <expected>
#include <nlohmann/json.hpp>
#include <string>

namespace smart_home::tui {

std::expected<nlohmann::json, std::string> loadConfig(const std::string &path,
                                                      const std::string &fallbackPath = {});
std::expected<void, std::string> saveConfig(const std::string &path, const nlohmann::json &config);

} // namespace smart_home::tui
