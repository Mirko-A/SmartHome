#pragma once

#include <QtCharts/QBarSet>
#include <QtCharts/QChart>

#include "../analytics_model.h"

class Histogram {
  public:
    Histogram(QtCharts::QChart &chart, const QString &title);
    void render(const OnTimeHistory &history);

  private:
    // Owned by the series, which is owned by the chart view's chart.
    QtCharts::QBarSet *m_bars;
};
