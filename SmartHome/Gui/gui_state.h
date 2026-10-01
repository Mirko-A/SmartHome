#pragma once
#include <optional>

#include "home_settings.h"

namespace smart_home::gui {
enum class Light { LIVING_ROOM, BEDROOM, KITCHEN };
enum class SpeakerControl { VOLUME, BASS, PITCH };

struct GuiState {
    HomeSettings pending;
    HomeSettings saved;
    // No device application path exists yet. Never infer applied state from a save.
    std::optional<HomeSettings> applied;
    bool loaded = false;
    bool busy = false;
    bool loading = false;
};
} // namespace smart_home::gui
