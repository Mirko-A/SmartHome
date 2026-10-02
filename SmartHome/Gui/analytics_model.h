#ifndef ANALYTICS_MODEL_H
#define ANALYTICS_MODEL_H

#include <QElapsedTimer>
#include <QPair>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
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

    void update(bool requestedOn, qint64 elapsedMs);

  private:
    void shift();

  public:
    // Not owned by histogram. It is created and then
    // appended to a chart. From then on, the chart
    // is the owner of these objects.
    QtCharts::QBarSeries *barSeries;
    // Borrowed: owned by barSeries
    QtCharts::QBarSet *m_barSet;

  private:
    qint64 m_lastUpdateMs = -1;
    qint64 m_currentHour = 0;
    bool m_requestedOn = false;
};

class LineGraph {
  public:
    LineGraph(QtCharts::QValueAxis &axisX, QtCharts::QValueAxis &axisY);

    void update(int16_t newValue, qint64 elapsedMs);

  public:
    // Borrowed: the chart owns the series and axes and must outlive this wrapper.
    QtCharts::QLineSeries *lineSeries;

  private:
    QtCharts::QValueAxis &m_axisX;
    QtCharts::QValueAxis &m_axisY;
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

    void updateHistograms(const HomeSettings &homeCfg);

    void updateLineGraphs(const HomeSettings &homeCfg);

    std::unique_ptr<Histogram> createChartWithHistogram(QtCharts::QChart &chart, QString title,
                                                        const QStringList &rangeX,
                                                        QPair<size_t, size_t> rangeY);
    std::unique_ptr<LineGraph> createChartWithLineGraph(QtCharts::QChart &chart, QString title,
                                                        QString axisTitle);

    AnalyticsData m_analyticsData;
    QElapsedTimer m_historyClock;
    qint64 m_lastLineSampleMs = -1;
};

#endif // ANALYTICS_MODEL_H
