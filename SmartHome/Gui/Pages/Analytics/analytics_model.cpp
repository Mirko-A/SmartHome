#include "analytics_model.h"

#include <algorithm>

namespace {
constexpr int64_t HOUR_MS = 3600000;
constexpr size_t MAX_BUCKETS = 24;
constexpr size_t MAX_SAMPLES = 3600;
} // namespace
void OnTimeHistory::update(bool on, int64_t elapsedMs) {
    const int64_t hour = elapsedMs / HOUR_MS;
    const auto shifts = std::min(hour - currentHour, int64_t(MAX_BUCKETS));
    for (int64_t i = 0; i < shifts; ++i) {
        if (minutes.size() == MAX_BUCKETS)
            minutes.pop_front();
        minutes.push_back(0);
    }
    currentHour = hour;
    if (lastUpdateMs >= 0 && requestedOn) {
        const int64_t firstHour = hour - static_cast<int64_t>(minutes.size()) + 1;
        for (size_t i = 0; i < minutes.size(); ++i) {
            const int64_t bucketStart = (firstHour + static_cast<int64_t>(i)) * HOUR_MS;
            const auto start = std::max(lastUpdateMs, bucketStart);
            const auto end = std::min(elapsedMs, bucketStart + HOUR_MS);
            if (end > start)
                minutes[i] += (end - start) / 60000.0;
        }
    }
    lastUpdateMs = elapsedMs;
    requestedOn = on;
}
void AnalyticsModel::update(const HomeSettings &settings, int64_t elapsedMs) {
    const auto lights = settings.lights();
    const std::array<bool, 4> requests{lights.livingRoomLightOn, lights.bedroomLightOn,
                                       lights.kitchenLightOn, settings.ac().on};
    for (size_t i = 0; i < requests.size(); ++i)
        m_onTime[i].update(requests[i], elapsedMs);
    if (m_lastSampleMs >= 0 && elapsedMs - m_lastSampleMs < 1000)
        return;
    m_lastSampleMs = elapsedMs;
    const double seconds = elapsedMs / 1000.0;
    while (!m_samples.empty() &&
           (m_samples.size() >= MAX_SAMPLES || m_samples.front().seconds <= seconds - 3600))
        m_samples.pop_front();
    m_samples.push_back({seconds, settings.sensors()});
}
