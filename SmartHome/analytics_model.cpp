#include "analytics_model.h"

#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QStringList>

// TODO: For testing it is reduced to 60
// #define ONE_HOUR_IN_SEC 3600
#define ONE_HOUR_IN_SEC 60
#define X_AXIS_POS 0

static constexpr size_t MAX_HISTOGRAM_VALUE = ONE_HOUR_IN_SEC;
static constexpr size_t MAX_BARSET_COUNT = 24;

// TODO: Check if this is too few/many points after ONE_SEC_IN_TICKS
// is set back to real value (found in mainwindow.cpp)
static constexpr unsigned int MAX_LINE_GRAPH_POINTS_INITIAL = 100;

Histogram::Histogram(QString name)
    : barSeries(new QtCharts::QBarSeries), m_barSet(new QtCharts::QBarSet(name)), m_valueCounter(0)
{
    *m_barSet << 0;

    barSeries->append(m_barSet);
}

Histogram::~Histogram()
{
    delete m_barSet;
}

void Histogram::update()
{
    // Histograms are updated periodically and represent
    // number of units of time spent in a particular state
    // e.g. Number of seconds a light was turned on
    // That's why we update the value counter, to signal
    // that another unit of time has passed, and replace
    // the current bar set with this new value
    m_barSet->replace(m_barSet->count() - 1, ++m_valueCounter);
}

void Histogram::shift()
{
    // Once in a while (e.g. once per hour) the histogram
    // needs to be shifted to the left in order to start
    // counting the amount of time spent in a particular
    // state for this new period (e.g. for the next hour)
    m_valueCounter = 0;
    *m_barSet << 0;

    // Once the maximum desired number of bar sets is reached
    // we should remove the least recent one to keep the number
    // of bar sets constant
    if(m_barSet->count() > static_cast<int>(MAX_BARSET_COUNT))
    {
        m_barSet->remove(0);
    }
}

LineGraph::LineGraph(QString title, unsigned int initialMaxPointsAllowed)
    : lineSeries(new QtCharts::QLineSeries()), m_title(title), m_pointCount(0), m_maxPointsAllowed(initialMaxPointsAllowed)
{

}

void LineGraph::update(int16_t newValue)
{
    // Line graphs will be updated periodically by adding a new
    // point once per cycle (e.g. every second).
    *lineSeries << QPointF(m_pointCount++, newValue);
    expandLineSeriesIfNeeded();
}

void LineGraph::update(int newValue)
{
    // Line graphs will be updated periodically by adding a new
    // point once per cycle (e.g. every second).
    *lineSeries << QPointF(m_pointCount++, newValue);
    expandLineSeriesIfNeeded();
}

void LineGraph::expandLineSeriesIfNeeded()
{
    /* Whole X-axis of the line graph is filled out and needs to be expanded
     * In case we reached the maximum range, we will freeze it
     * at [UINT_MAX/10; UINT_MAX] (through the condition after &&). */
    if(static_cast<size_t>(lineSeries->count()) >= m_maxPointsAllowed &&
       m_maxPointsAllowed != UINT_MAX)
    {
        /* Calculate new maximum for the X-axis:
         *     case 1: maxLineGraphPoints*10 > UINT_MAX -> overflow would occur if we multiply by 10
         *                                                 so we just set maxLineGraphPoints to UINT_MAX
         *     case 2: maxLineGraphPoints*10 < UINT_MAX -> no overflow will occur so we can multiply
         *                                                 maxLineGraphPoints by 10 */
        m_maxPointsAllowed = ((UINT_MAX/10) < m_maxPointsAllowed) ? UINT_MAX : m_maxPointsAllowed * 10;

        lineSeries->attachedAxes().at(X_AXIS_POS)->setRange(0, m_maxPointsAllowed);
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
    initChartsWithLineGraph();
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

void AnalyticsModel::initChartsWithLineGraph()
{
    {
        auto ACTemperatureChartWithLineGraph = createChartWithLineGraph("AC Temperature",
                                                                        QPair<size_t, size_t>(0, MAX_LINE_GRAPH_POINTS_INITIAL),
                                                                        QPair<size_t, size_t>(MIN_AC_TEMP, MAX_AC_TEMP));
        m_ACTemperatureChart = ACTemperatureChartWithLineGraph.first;
        m_analyticsData->lineGraphs->ACTemperature = ACTemperatureChartWithLineGraph.second;
    }
}

QPair<QtCharts::QChart*, Histogram*> AnalyticsModel::createChartWithHistogram(QString title,
                                                                              QPair<size_t, size_t> rangeX,
                                                                              QPair<size_t, size_t> rangeY)
{
    QtCharts::QChart* chart = new QtCharts::QChart;
    Histogram* histogram = new Histogram(title);

    chart->setTitle(title);
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
    chart->legend()->hide();

    return QPair<QtCharts::QChart*, Histogram*>(chart, histogram);
}

QPair<QtCharts::QChart*, LineGraph*> AnalyticsModel::createChartWithLineGraph(QString title,
                                                                              QPair<int, int> rangeX,
                                                                              QPair<int, int> rangeY)
{
    QtCharts::QChart *chart = new QtCharts::QChart();
    LineGraph* graph = new LineGraph(title, rangeX.second);

    chart->setTitle(title);
    auto axisX = new QtCharts::QValueAxis;
    auto axisY = new QtCharts::QValueAxis;
    axisX->setRange(rangeX.first, rangeX.second);
    axisY->setRange(rangeY.first, rangeY.second);
    chart->addSeries(graph->lineSeries);
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    graph->lineSeries->attachAxis(axisX);
    graph->lineSeries->attachAxis(axisY);
    chart->legend()->hide();

    return QPair<QtCharts::QChart*, LineGraph*>(chart, graph);
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

void AnalyticsModel::updateLineGraphs(const HomeConfig& homeCfg)
{
    m_analyticsData->lineGraphs->ACTemperature->update(homeCfg.AC.temperature);
}

void AnalyticsModel::updateAnalyticsData(const HomeConfig& homeCfg)
{
    if (histogramTickCount == MAX_HISTOGRAM_VALUE)
    {
        shiftHistograms();
        histogramTickCount = 0;
    }

    updateHistograms(homeCfg);
    updateLineGraphs(homeCfg);

    histogramTickCount++;
}
