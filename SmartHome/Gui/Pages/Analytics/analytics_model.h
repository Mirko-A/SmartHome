#pragma once

#include <array>
#include <cstdint>
#include <deque>

#include "home_settings.h"

struct OnTimeHistory {
    std::deque<double> minutes{0.0};

    int64_t currentHour = 0;

    int64_t lastUpdateMs = -1;
    bool requestedOn = false;

    void update(bool on, int64_t elapsedMs);
};

struct SensorSample {
    double seconds;
    SensorReadings readings;
};

// Value-only history: no charts, series, axes, or widget lifetime dependencies.
class AnalyticsModel {
  public:
    void update(const HomeSettings &settings, int64_t elapsedMs);

    const std::array<OnTimeHistory, 4> &onTime() const {
        return m_onTime;
    }

    const std::deque<SensorSample> &samples() const {
        return m_samples;
    }

  private:
    std::array<OnTimeHistory, 4> m_onTime;
    std::deque<SensorSample> m_samples;
    int64_t m_lastSampleMs = -1;
};
