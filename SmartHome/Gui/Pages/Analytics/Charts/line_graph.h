#pragma once
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "../analytics_model.h"
class LineGraph {
  public:
    LineGraph(QtCharts::QChart &chart, const QString &title, int16_t SensorReadings::*reading);
    void render(const std::deque<SensorSample> &samples);

  private:
    // All Qt objects are owned by the chart.
    QtCharts::QLineSeries *m_series;
    QtCharts::QValueAxis *m_x;
    QtCharts::QValueAxis *m_y;
    int16_t SensorReadings::*m_reading;
    double m_lastSample = -1;
};
