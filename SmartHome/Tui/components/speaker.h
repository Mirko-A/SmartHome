#pragma once

#include <ftxui/component/component.hpp>

#include "tui_state.h"

namespace smart_home::tui::components {

// Referenced state must outlive the returned component.
ftxui::Component makeSpeakersPanel(TuiState &state);

} // namespace smart_home::tui::components
