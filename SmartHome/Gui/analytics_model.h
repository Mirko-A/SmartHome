#ifndef ANALYTICS_MODEL_H
#define ANALYTICS_MODEL_H

#include <QPair>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <memory>

#include "home_settings.h"

enum class AnalyticsPage {
    LIGHTS = 0,
    AC,
    SENSORS,
};

class Histogram {
  public:
    Histogram(QString name);

    void update();
    void shift();

  public:
    // Not owned by histogram. It is created and then
    // appended to a chart. From then on, the chart
    // is the owner of these objects.
    QtCharts::QBarSeries *barSeries;
    // Borrowed: owned by barSeries
    QtCharts::QBarSet *m_barSet;

  private:
    // Represents the current value of the bar
    size_t m_valueCounter;
};

class LineGraph {
  public:
    LineGraph(QString title, unsigned int initialMaxPointsAllowed);

    void update(int16_t newValue);
    void update(int newValue);

  private:
    void expandLineSeriesIfNeeded();

  public:
    // Not owned by histogram. It is created and then
    // appended to a chart. From then on, the chart
    // is the owner of these objects.
    QtCharts::QLineSeries *lineSeries;

  private:
    // Represents the number of times the Line graph
    // has been updated. Used as the X-axis
    QString m_title;
    unsigned int m_pointCount;
    unsigned int m_maxPointsAllowed;
};

// The supplied charts belong to the views and must outlive the model.
struct AnalyticsCharts {
    QtCharts::QChart *livingRoomLight;
    QtCharts::QChart *bedroomLight;
    QtCharts::QChart *kitchenLight;
    QtCharts::QChart *ACOn;
    QtCharts::QChart *temperatureSensor;
    QtCharts::QChart *humiditySensor;
    QtCharts::QChart *brightnessSensor;
};

struct Histograms {
    std::unique_ptr<Histogram> livingRoomLight;
    std::unique_ptr<Histogram> bedroomLight;
    std::unique_ptr<Histogram> kitchenLight;
    std::unique_ptr<Histogram> acOn;
};

struct LineGraphs {
    std::unique_ptr<LineGraph> temperatureSensor;
    std::unique_ptr<LineGraph> humiditySensor;
    std::unique_ptr<LineGraph> brightnessSensor;
};

struct AnalyticsData {
    Histograms histograms;
    LineGraphs lineGraphs;
};

class AnalyticsModel {
  public:
    explicit AnalyticsModel(const AnalyticsCharts &charts);

  public:
    void updateAnalyticsData(const HomeSettings &homeCfg);

  private:
    void initChartsWithHistogram(const AnalyticsCharts &charts);
    void initChartsWithLineGraph(const AnalyticsCharts &charts);

    void shiftHistograms();
    void updateHistograms(const HomeSettings &homeCfg);

    void updateLineGraphs(const HomeSettings &homeCfg);

    std::unique_ptr<Histogram> createChartWithHistogram(QtCharts::QChart &chart, QString title,
                                                        const QStringList &rangeX,
                                                        QPair<size_t, size_t> rangeY);
    std::unique_ptr<LineGraph> createChartWithLineGraph(QtCharts::QChart &chart, QString title,
                                                        QPair<int, int> rangeX,
                                                        QPair<int, int> rangeY);

    AnalyticsData m_analyticsData;
    size_t histogramTickCount = 0;
};

#endif // ANALYTICS_MODEL_H
