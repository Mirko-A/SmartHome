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
    // Owned by analytics model
    m_livingRoomLightChart = new QtCharts::QChart;
    m_bedroomLightChart = new QtCharts::QChart;
    m_kitchenLightChart = new QtCharts::QChart;
    m_ACOnChart = new QtCharts::QChart;
    m_ACTemperatureChart = new QtCharts::QChart;
    m_ACModeChart = new QtCharts::QChart;
    m_temperatureSensorChart = new QtCharts::QChart;
    m_humiditySensorChart = new QtCharts::QChart;
    m_brightnessSensorChart = new QtCharts::QChart;

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
        auto livingRoomLightAxisX = new QtCharts::QValueAxis;
        auto livingRoomLightAxisY = new QtCharts::QValueAxis;
        livingRoomLightAxisX->setRange(0, MAX_BARSET_COUNT);
        livingRoomLightAxisY->setRange(0, MAX_HISTOGRAM_VALUE);
        m_analyticsData->histograms.livingRoomLight = new Histogram("Living room light on per hour");
        m_livingRoomLightChart->addSeries(m_analyticsData->histograms.livingRoomLight->barSeries);
        m_livingRoomLightChart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
        m_livingRoomLightChart->addAxis(livingRoomLightAxisX, Qt::AlignBottom);
        m_livingRoomLightChart->addAxis(livingRoomLightAxisY, Qt::AlignLeft);
        m_analyticsData->histograms.livingRoomLight->barSeries->attachAxis(livingRoomLightAxisX);
        m_analyticsData->histograms.livingRoomLight->barSeries->attachAxis(livingRoomLightAxisY);
        m_livingRoomLightChart->legend()->setVisible(true);
        m_livingRoomLightChart->legend()->setAlignment(Qt::AlignBottom);
    }

    {
        auto bedroomLightAxisX = new QtCharts::QValueAxis;
        auto bedroomLightAxisY = new QtCharts::QValueAxis;
        bedroomLightAxisX->setRange(0, MAX_BARSET_COUNT);
        bedroomLightAxisY->setRange(0, MAX_HISTOGRAM_VALUE);
        m_analyticsData->histograms.bedroomLight = new Histogram("Bedroom room light on per hour");
        m_bedroomLightChart->addSeries(m_analyticsData->histograms.bedroomLight->barSeries);
        m_bedroomLightChart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
        m_bedroomLightChart->addAxis(bedroomLightAxisX, Qt::AlignBottom);
        m_bedroomLightChart->addAxis(bedroomLightAxisY, Qt::AlignLeft);
        m_analyticsData->histograms.bedroomLight->barSeries->attachAxis(bedroomLightAxisX);
        m_analyticsData->histograms.bedroomLight->barSeries->attachAxis(bedroomLightAxisY);
        m_bedroomLightChart->legend()->setVisible(true);
        m_bedroomLightChart->legend()->setAlignment(Qt::AlignBottom);
    }

    {
        auto kitchenLightAxisX = new QtCharts::QValueAxis;
        auto kitchenLightAxisY = new QtCharts::QValueAxis;
        kitchenLightAxisX->setRange(0, MAX_BARSET_COUNT);
        kitchenLightAxisY->setRange(0, MAX_HISTOGRAM_VALUE);
        m_analyticsData->histograms.kitchenLight = new Histogram("Kitchen room light on per hour");
        m_kitchenLightChart->addSeries(m_analyticsData->histograms.kitchenLight->barSeries);
        m_kitchenLightChart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
        m_kitchenLightChart->addAxis(kitchenLightAxisX, Qt::AlignBottom);
        m_kitchenLightChart->addAxis(kitchenLightAxisY, Qt::AlignLeft);
        m_analyticsData->histograms.kitchenLight->barSeries->attachAxis(kitchenLightAxisX);
        m_analyticsData->histograms.kitchenLight->barSeries->attachAxis(kitchenLightAxisY);
        m_kitchenLightChart->legend()->setVisible(true);
        m_kitchenLightChart->legend()->setAlignment(Qt::AlignBottom);
    }

    {
        auto ACOnAxisX = new QtCharts::QValueAxis;
        auto ACOnAxisY = new QtCharts::QValueAxis;
        ACOnAxisX->setRange(0, MAX_BARSET_COUNT);
        ACOnAxisY->setRange(0, MAX_HISTOGRAM_VALUE);
        m_analyticsData->histograms.ACOn = new Histogram("AC on per hour");
        m_ACOnChart->addSeries(m_analyticsData->histograms.ACOn->barSeries);
        m_ACOnChart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
        m_ACOnChart->addAxis(ACOnAxisX, Qt::AlignBottom);
        m_ACOnChart->addAxis(ACOnAxisY, Qt::AlignLeft);
        m_analyticsData->histograms.ACOn->barSeries->attachAxis(ACOnAxisX);
        m_analyticsData->histograms.ACOn->barSeries->attachAxis(ACOnAxisY);
        m_ACOnChart->legend()->setVisible(true);
        m_ACOnChart->legend()->setAlignment(Qt::AlignBottom);
    }
}

void AnalyticsModel::shiftHistograms()
{
    m_analyticsData->histograms.livingRoomLight->shift();
    m_analyticsData->histograms.bedroomLight->shift();
    m_analyticsData->histograms.kitchenLight->shift();
    m_analyticsData->histograms.ACOn->shift();
}

void AnalyticsModel::updateHistograms(const HomeConfig& homeCfg)
{
    if(homeCfg.lights.livingRoomLightOn)
    {
        m_analyticsData->histograms.livingRoomLight->update();
    }
    if(homeCfg.lights.bedroomLightOn)
    {
        m_analyticsData->histograms.bedroomLight->update();
    }
    if(homeCfg.lights.kitchenLightOn)
    {
        m_analyticsData->histograms.kitchenLight->update();
    }
    if(homeCfg.AC.on)
    {
        m_analyticsData->histograms.ACOn->update();
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
