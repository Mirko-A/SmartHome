#include "analytics_model.h"

#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QStringList>

AnalyticsModel::AnalyticsModel()
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

    initCharts();
}

void AnalyticsModel::initCharts()
{
    initLightCharts();
    initACCharts();
    initSensorCharts();
}

void AnalyticsModel::initLightCharts()
{
    auto set0 = new QtCharts::QBarSet("Jane");
    auto set1 = new QtCharts::QBarSet("John");
    auto set2 = new QtCharts::QBarSet("Axel");
    auto set3 = new QtCharts::QBarSet("Mary");
    auto set4 = new QtCharts::QBarSet("Samantha");

    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
    *set1 << 5 << 0 << 0 << 4 << 0 << 7;
    *set2 << 3 << 5 << 8 << 13 << 8 << 5;
    *set3 << 5 << 6 << 7 << 3 << 4 << 5;
    *set4 << 9 << 7 << 5 << 3 << 1 << 2;

    QtCharts::QBarSeries *series = new QtCharts::QBarSeries;
    series->append(set0);
    series->append(set1);
    series->append(set2);
    series->append(set3);
    series->append(set4);

    m_livingRoomLightChart->addSeries(series);
    m_livingRoomLightChart->setTitle("Simple Bar Chart");
    m_livingRoomLightChart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    QStringList categories {"Jan", "Feb", "Mar", "Apr", "May", "Jun"};
    auto axisX = new QtCharts::QBarCategoryAxis;
    axisX->append(categories);
    m_livingRoomLightChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto axisY = new QtCharts::QValueAxis;
    axisY->setRange(0,15);
    m_livingRoomLightChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    m_livingRoomLightChart->legend()->setVisible(true);
    m_livingRoomLightChart->legend()->setAlignment(Qt::AlignBottom);
}

void AnalyticsModel::initACCharts()
{

}

void AnalyticsModel::initSensorCharts()
{

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
}
