#ifndef ANALYTICS_MODEL_H
#define ANALYTICS_MODEL_H

#include <QtCharts/QChart>

#include <QTimer>

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

    void initCharts();
private:
    void initLightCharts();
    void initACCharts();
    void initSensorCharts();

    void onUpdate();

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
};

#endif // ANALYTICS_MODEL_H
