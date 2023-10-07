#ifndef ANALYTICS_MODEL_H
#define ANALYTICS_MODEL_H

#include "home_config.h"

#include <QtCharts/QChart>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>

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

    // This function is supposed to increment the value
    // and, when needed, update the BarSet and BarSeries
    void update();
    void shift();

public:
    // Not owned by histogram. It is created and then
    // appended to a chart. From then on, the chart
    // is the owner of these objects.
    QtCharts::QBarSeries* barSeries;
    QtCharts::QBarSet* barSet;
private:
    size_t m_valueCounter;
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

struct AnalyticsData
{
    Histograms* histograms;

    AnalyticsData()
        : histograms(new Histograms)
    {

    }

    ~AnalyticsData()
    {
        delete histograms;
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
    void shiftHistograms();
    void updateHistograms(const HomeConfig& homeCfg);

    void onUpdate();

private:
    QPair<QtCharts::QChart*, Histogram*> createChartWithHistogram(QString name, QPair<size_t, size_t> rangeX, QPair<size_t, size_t> rangeY);

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
