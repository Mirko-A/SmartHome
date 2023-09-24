#ifndef ANALYTICS_MODEL_H
#define ANALYTICS_MODEL_H

#include <QPushButton>
#include <QtCharts/QChart>

#include <QAbstractItemModel>

enum class AnalyticsPage
{
    LIGHTS = 0,
    AC,
    SENSORS,
};

// class AnalyticsModel : public QAbstractItemModel
class AnalyticsModel
{
    //Q_OBJECT
public:
    AnalyticsModel();
    ~AnalyticsModel();

private:
    void initCharts();
    void initLightCharts();
    void initACCharts();
    void initSensorCharts();

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

    QPushButton* m_analyticsPageLightsBtn;
    QPushButton* m_analyticsPageACBtn;
    QPushButton* m_analyticsPageSensorsBtn;
};

#endif // ANALYTICS_MODEL_H
