#include "analytics_model.h"

#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QStringList>

// TODO: For testing it is reduced to 60
// #define ONE_HOUR_IN_SEC 3600
#define ONE_HOUR_IN_SEC 60

constexpr size_t MAX_HISTOGRAM_VALUE = ONE_HOUR_IN_SEC;
constexpr size_t MAX_BARSET_COUNT = 24;

Histogram::Histogram(QString name)
    : m_valueCounter(0)
{
    barSet = new QtCharts::QBarSet(name);
    *barSet << 0;
    barSeries = new QtCharts::QBarSeries;

    barSeries->append(barSet);
}

void Histogram::update()
{
    barSet->replace(barSet->count() - 1, ++m_valueCounter);
}

void Histogram::shift()
{
    m_valueCounter = 0;
    *barSet << 0;

    if(barSet->count() > static_cast<int>(MAX_BARSET_COUNT))
    {
        barSet->remove(0);
    }
}

AnalyticsModel::AnalyticsModel()
    : histogramTickCount(0)
{
    m_analyticsData = new AnalyticsData;
    initCharts();
}

AnalyticsModel::~AnalyticsModel()
{
    delete m_livingRoomLightChart;
    delete m_bedroomLightChart;
    delete m_kitchenLightChart;
    delete m_ACOnChart;
    delete m_ACTemperatureChart;
    delete m_ACModeChart;
    delete m_temperatureSensorChart;
    delete m_humiditySensorChart;
    delete m_brightnessSensorChart;

    delete m_analyticsData;
}

void AnalyticsModel::initCharts()
{
    initChartsWithHistogram();
}

void AnalyticsModel::initChartsWithHistogram()
{
    {
        auto livingRoomChartWithHistogram = createChartWithHistogram("Living room light on per hour",
                                                        QPair<size_t, size_t>(0, MAX_BARSET_COUNT),
                                                        QPair<size_t, size_t>(0, MAX_HISTOGRAM_VALUE));

        m_livingRoomLightChart = livingRoomChartWithHistogram.first;
        m_analyticsData->histograms->livingRoomLight = livingRoomChartWithHistogram.second;
    }

    {
        auto bedroomChartWithHistogram = createChartWithHistogram("Bedroom light on per hour",
                                                                  QPair<size_t, size_t>(0, MAX_BARSET_COUNT),
                                                                  QPair<size_t, size_t>(0, MAX_HISTOGRAM_VALUE));

        m_bedroomLightChart = bedroomChartWithHistogram.first;
        m_analyticsData->histograms->bedroomLight = bedroomChartWithHistogram.second;
    }

    {
        auto kitchenChartWithHistogram = createChartWithHistogram("Kitchen light on per hour",
                                                                  QPair<size_t, size_t>(0, MAX_BARSET_COUNT),
                                                                  QPair<size_t, size_t>(0, MAX_HISTOGRAM_VALUE));

        m_kitchenLightChart = kitchenChartWithHistogram.first;
        m_analyticsData->histograms->kitchenLight = kitchenChartWithHistogram.second;
    }

    {
        auto ACOnChartWithHistogram = createChartWithHistogram("AC on per hour",
                                                               QPair<size_t, size_t>(0, MAX_BARSET_COUNT),
                                                               QPair<size_t, size_t>(0, MAX_HISTOGRAM_VALUE));

        m_ACOnChart = ACOnChartWithHistogram.first;
        m_analyticsData->histograms->ACOn = ACOnChartWithHistogram.second;
    }
}

QPair<QtCharts::QChart*, Histogram*> AnalyticsModel::createChartWithHistogram(QString name, QPair<size_t, size_t> rangeX, QPair<size_t, size_t> rangeY)
{
    QtCharts::QChart* chart = new QtCharts::QChart;
    Histogram* histogram = new Histogram(name);

    auto axisX = new QtCharts::QValueAxis;
    auto axisY = new QtCharts::QValueAxis;
    axisX->setRange(rangeX.first, rangeX.second);
    axisY->setRange(rangeY.first, rangeY.second);
    chart->addSeries(histogram->barSeries);
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    histogram->barSeries->attachAxis(axisX);
    histogram->barSeries->attachAxis(axisY);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    return QPair<QtCharts::QChart*, Histogram*>(chart, histogram);
}

void AnalyticsModel::shiftHistograms()
{
    m_analyticsData->histograms->livingRoomLight->shift();
    m_analyticsData->histograms->bedroomLight->shift();
    m_analyticsData->histograms->kitchenLight->shift();
    m_analyticsData->histograms->ACOn->shift();
}

void AnalyticsModel::updateHistograms(const HomeConfig& homeCfg)
{
    if(homeCfg.lights.livingRoomLightOn)
    {
        m_analyticsData->histograms->livingRoomLight->update();
    }
    if(homeCfg.lights.bedroomLightOn)
    {
        m_analyticsData->histograms->bedroomLight->update();
    }
    if(homeCfg.lights.kitchenLightOn)
    {
        m_analyticsData->histograms->kitchenLight->update();
    }
    if(homeCfg.AC.on)
    {
        m_analyticsData->histograms->ACOn->update();
    }
}

void AnalyticsModel::updateAnalyticsData(const HomeConfig& homeCfg)
{
    if ( (histogramTickCount > 0) &&
         ((histogramTickCount % MAX_HISTOGRAM_VALUE) == 0) )
        shiftHistograms();

    updateHistograms(homeCfg);

    histogramTickCount++;
}
