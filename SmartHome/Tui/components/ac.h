#pragma once

#include <ftxui/component/component.hpp>

#include "app_state.h"

namespace smart_home::tui::components {

// Referenced state must outlive the returned component.
ftxui::Component makeAcPanel(app::AppState &state);

} // namespace smart_home::tui::components
