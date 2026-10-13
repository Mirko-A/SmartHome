#pragma once

#include <ftxui/dom/elements.hpp>

#include "app_state.h"

namespace smart_home::tui::components {

ftxui::Element renderSensors(const app::AppState &state);

} // namespace smart_home::tui::components
