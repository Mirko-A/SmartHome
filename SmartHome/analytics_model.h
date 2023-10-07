#ifndef ANALYTICS_MODEL_H
#define ANALYTICS_MODEL_H

#include "home_config.h"

#include <QtCharts/QChart>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QLineSeries>

#include <QPair>

enum class AnalyticsPage
{
    LIGHTS = 0,
    AC,
    SENSORS,
};

class Histogram
{
public:
    Histogram(QString name);
    ~Histogram();

    void update();
    void shift();

public:
    // Not owned by histogram. It is created and then
    // appended to a chart. From then on, the chart
    // is the owner of these objects.
    QtCharts::QBarSeries* barSeries;
    // Owned by histogram
    QtCharts::QBarSet* m_barSet;
private:
    // Represents the current value of the bar
    size_t m_valueCounter;
};

class LineGraph
{
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
    QtCharts::QLineSeries* lineSeries;
private:
    // Represents the number of times the Line graph
    // has been updated. Used as the X-axis
    QString m_title;
    unsigned int m_pointCount;
    unsigned int m_maxPointsAllowed;
};

struct Histograms
{
    Histogram* livingRoomLight;
    Histogram* bedroomLight;
    Histogram* kitchenLight;
    Histogram* ACOn;

    ~Histograms()
    {
        delete livingRoomLight;
        delete bedroomLight;
        delete kitchenLight;
        delete ACOn;
    }
};

struct LineGraphs
{
    LineGraph* ACTemperature;

    ~LineGraphs()
    {
        delete ACTemperature;
    }
};

struct AnalyticsData
{
    Histograms* histograms;
    LineGraphs* lineGraphs;

    AnalyticsData()
        : histograms(new Histograms), lineGraphs(new LineGraphs)
    {

    }

    ~AnalyticsData()
    {
        delete histograms;
        delete lineGraphs;
    }
};

class AnalyticsModel
{
public:
    AnalyticsModel();
    ~AnalyticsModel();

    void initCharts();

public:
    void updateAnalyticsData(const HomeConfig& homeCfg);

private:
    void initChartsWithHistogram();
    void initChartsWithLineGraph();

    void shiftHistograms();
    void updateHistograms(const HomeConfig& homeCfg);

    void updateLineGraphs(const HomeConfig& homeCfg);

    void onUpdate();

private:
    QPair<QtCharts::QChart*, Histogram*> createChartWithHistogram(QString title,
                                                                  QPair<size_t, size_t> rangeX,
                                                                  QPair<size_t, size_t> rangeY);
    QPair<QtCharts::QChart*, LineGraph*> createChartWithLineGraph(QString title,
                                                                  QPair<int, int> rangeX,
                                                                  QPair<int, int> rangeY);
public:
    QtCharts::QChart* m_livingRoomLightChart;
    QtCharts::QChart* m_bedroomLightChart;
    QtCharts::QChart* m_kitchenLightChart;

    QtCharts::QChart* m_ACOnChart;
    QtCharts::QChart* m_ACTemperatureChart;
    QtCharts::QChart* m_ACModeChart;

    QtCharts::QChart* m_temperatureSensorChart;
    QtCharts::QChart* m_humiditySensorChart;
    QtCharts::QChart* m_brightnessSensorChart;

private:
    AnalyticsData* m_analyticsData;
    size_t histogramTickCount;
};

#endif // ANALYTICS_MODEL_H
