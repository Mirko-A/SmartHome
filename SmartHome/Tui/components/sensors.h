#pragma once

#include <ftxui/dom/elements.hpp>

#include "tui_state.h"

namespace smart_home::tui::components {

ftxui::Element renderSensors(const TuiState &state);

} // namespace smart_home::tui::components
