#include "analytics_model.h"

#include <algorithm>
#include <cassert>

namespace {

constexpr int64_t HOUR_MS = 3600000;
constexpr size_t MAX_BUCKETS = 24;
constexpr size_t MAX_SAMPLES = 3600;

} // namespace

void OnTimeHistory::update(bool on, int64_t elapsedMs) {
    const int64_t hour = elapsedMs / HOUR_MS;
    assert(hour >= currentHour);

    const auto shifts = std::min(hour - currentHour, int64_t(MAX_BUCKETS));
    while ((minutes.size() + shifts) > MAX_BUCKETS) {
        minutes.pop_front();
    }
    for (int64_t i = 0; i < shifts; i++) {
        minutes.push_back(0);
    }
    currentHour = hour;

    if (lastUpdateMs != -1 && requestedOn) {
        const int64_t firstHour = hour - static_cast<int64_t>(minutes.size()) + 1;
        for (size_t i = 0; i < minutes.size(); i++) {
            const int64_t bucketStart = (firstHour + static_cast<int64_t>(i)) * HOUR_MS;
            const auto start = std::max(lastUpdateMs, bucketStart);
            const auto end = std::min(elapsedMs, bucketStart + HOUR_MS);
            if (end > start) {
                minutes[i] += (end - start) / 60.0 / 1000.0;
            }
        }
    }

    lastUpdateMs = elapsedMs;
    requestedOn = on;
}

bool shouldDiscardSample(const std::deque<SensorSample> &samples, const double elapsedSecs) {
    if (samples.empty()) {
        return false;
    }
    return (samples.size() >= MAX_SAMPLES) || (samples.front().seconds <= (elapsedSecs - 3600));
}

void AnalyticsModel::update(const HomeSettings &settings, int64_t elapsedMs) {
    const auto lights = settings.lights();
    const std::array<bool, 4> requests{lights.livingRoomLightOn, lights.bedroomLightOn,
                                       lights.kitchenLightOn, settings.ac().on};

    for (size_t i = 0; i < requests.size(); ++i) {
        m_onTime[i].update(requests[i], elapsedMs);
    }

    const int64_t sinceLastUpdateMs = elapsedMs - m_lastSampleMs;
    if (m_lastSampleMs != -1 && sinceLastUpdateMs < 1000) {
        return;
    }
    m_lastSampleMs = elapsedMs;

    const double elapsedSecs = elapsedMs / 1000.0;
    while (shouldDiscardSample(m_samples, elapsedSecs)) {
        m_samples.pop_front();
    }
    m_samples.push_back({elapsedSecs, settings.sensors()});
}
