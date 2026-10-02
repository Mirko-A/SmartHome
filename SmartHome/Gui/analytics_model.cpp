#include "analytics_model.h"

#include <qnamespace.h>

#include <QBarCategoryAxis>
#include <QStringList>
#include <QValueAxis>
#include <algorithm>

#include "home_settings.h"

static const QColor CHART_BACKGROUND_COLOR = QColor(52, 59, 71);

static const QColor HISTOGRAM_BAR_COLOR = QColor(160, 110, 181);

static constexpr size_t MAX_HISTOGRAM_VALUE = 60;
static constexpr int MAX_BARSET_COUNT = 24;
static constexpr qint64 HOUR_MS = 60 * 60 * 1000;
static constexpr qreal MINUTE_MS = 60 * 1000;

static const QStringList HISTOGRAM_X_AXIS =
    QStringList{"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "10", "11", "12",
                "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24"};

static constexpr int MAX_LINE_GRAPH_POINTS = 3600;
static constexpr qint64 LINE_SAMPLE_INTERVAL_MS = 1000;
static constexpr qreal LINE_HISTORY_SECONDS = 3600;

static constexpr QColor LINE_GRAPH_COLOR = QColor(160, 110, 181);

Histogram::Histogram(QString name)
    : barSeries(new QtCharts::QBarSeries), m_barSet(new QtCharts::QBarSet(name)) {
    m_barSet->setColor(HISTOGRAM_BAR_COLOR);
    barSeries->append(m_barSet);
    barSeries->setBarWidth(1);

    // Initialize the first bar.
    *m_barSet << 0;
}

void Histogram::update(bool requestedOn, qint64 elapsedMs) {
    const qint64 currentHour = elapsedMs / HOUR_MS;
    // Keep work bounded even after a delay spanning more than the retained history.
    const int shifts =
        static_cast<int>(std::min(currentHour - m_currentHour, qint64(MAX_BARSET_COUNT)));
    for (int i = 0; i < shifts; ++i) {
        shift();
    }
    m_currentHour = currentHour;

    // The previous observed request applies until this observation. These are
    // session-hour buckets, not measured hardware operation or calendar hours.
    if (m_lastUpdateMs >= 0 && m_requestedOn) {
        const qint64 firstHour = currentHour - m_barSet->count() + 1;
        for (int i = 0; i < m_barSet->count(); ++i) {
            const qint64 bucketStart = (firstHour + i) * HOUR_MS;
            const qint64 start = std::max(m_lastUpdateMs, bucketStart);
            const qint64 end = std::min(elapsedMs, bucketStart + HOUR_MS);
            if (end > start) {
                m_barSet->replace(i, m_barSet->at(i) + (end - start) / MINUTE_MS);
            }
        }
    }
    m_lastUpdateMs = elapsedMs;
    m_requestedOn = requestedOn;
}

void Histogram::shift() {
    if (m_barSet->count() == MAX_BARSET_COUNT) {
        m_barSet->remove(0);
    }
    *m_barSet << 0;
}

LineGraph::LineGraph(QtCharts::QValueAxis &axisX, QtCharts::QValueAxis &axisY)
    : lineSeries(new QtCharts::QLineSeries()), m_axisX(axisX), m_axisY(axisY) {
    QPen lineGraphPen = lineSeries->pen();
    lineGraphPen.setBrush(QBrush(LINE_GRAPH_COLOR));
    lineGraphPen.setWidth(3);
    lineSeries->setPen(lineGraphPen);
}

void LineGraph::update(int16_t newValue, qint64 elapsedMs) {
    const qreal elapsedSeconds = elapsedMs / 1000.0;
    const qreal cutoff = elapsedSeconds - LINE_HISTORY_SECONDS;
    // Trim before appending so storage never exceeds the cap, even temporarily.
    int removeCount = std::max(0, lineSeries->count() - MAX_LINE_GRAPH_POINTS + 1);
    while (removeCount < lineSeries->count() && lineSeries->at(removeCount).x() <= cutoff) {
        ++removeCount;
    }
    if (removeCount > 0) {
        lineSeries->removePoints(0, removeCount);
    }
    lineSeries->append(elapsedSeconds, newValue);
    m_axisX.setRange(std::max(qreal(0), cutoff), std::max(LINE_HISTORY_SECONDS, elapsedSeconds));

    // Scale each sensor independently.
    qreal minimum = newValue;
    qreal maximum = newValue;
    for (int i = 0; i < lineSeries->count(); ++i) {
        minimum = std::min(minimum, lineSeries->at(i).y());
        maximum = std::max(maximum, lineSeries->at(i).y());
    }
    // A minimum padding also gives flat readings a non-degenerate axis.
    const qreal padding = std::max(qreal(1), (maximum - minimum) * 0.05);
    m_axisY.setRange(minimum - padding, maximum + padding);
}

AnalyticsModel::AnalyticsModel(const AnalyticsCharts &charts) {
    initChartsWithHistogram(charts);
    initChartsWithLineGraph(charts);
    m_historyClock.start();
}

void AnalyticsModel::initChartsWithHistogram(const AnalyticsCharts &charts) {
    m_analyticsData.histograms.livingRoomLight = createChartWithHistogram(
        *charts.livingRoomLight, "Living room", HISTOGRAM_X_AXIS, {0, MAX_HISTOGRAM_VALUE});
    m_analyticsData.histograms.bedroomLight = createChartWithHistogram(
        *charts.bedroomLight, "Bedroom", HISTOGRAM_X_AXIS, {0, MAX_HISTOGRAM_VALUE});
    m_analyticsData.histograms.kitchenLight = createChartWithHistogram(
        *charts.kitchenLight, "Kitchen", HISTOGRAM_X_AXIS, {0, MAX_HISTOGRAM_VALUE});
    m_analyticsData.histograms.acOn =
        createChartWithHistogram(*charts.ACOn, "AC", HISTOGRAM_X_AXIS, {0, MAX_HISTOGRAM_VALUE});
}

void AnalyticsModel::initChartsWithLineGraph(const AnalyticsCharts &charts) {
    // TODO: specify units once sensors are wired in.
    m_analyticsData.lineGraphs.temperatureSensor = createChartWithLineGraph(
        *charts.temperatureSensor, "Temperature", "Temperature (unit unspecified)");
    m_analyticsData.lineGraphs.humiditySensor =
        createChartWithLineGraph(*charts.humiditySensor, "Humidity", "Humidity (unit unspecified)");
    m_analyticsData.lineGraphs.brightnessSensor = createChartWithLineGraph(
        *charts.brightnessSensor, "Brightness", "Brightness (unit unspecified)");
}

std::unique_ptr<Histogram> AnalyticsModel::createChartWithHistogram(QtCharts::QChart &chart,
                                                                    QString title,
                                                                    const QStringList &rangeX,
                                                                    QPair<size_t, size_t> rangeY) {
    QFont titleFont = chart.titleFont();
    titleFont.setPointSize(16);
    chart.setTitleFont(titleFont);
    chart.setTitleBrush(QBrush(Qt::white));
    chart.setBackgroundBrush(QBrush(CHART_BACKGROUND_COLOR));

    auto histogram = std::make_unique<Histogram>(title);

    chart.setTitle(title);
    auto axisX = new QtCharts::QBarCategoryAxis;
    auto axisY = new QtCharts::QValueAxis;
    axisX->append(rangeX);
    axisX->setTitleText("Hourly buckets (oldest to newest)");
    axisX->setTitleBrush(QBrush(Qt::white));
    axisY->setTitleText("On-time (min)");
    axisY->setTitleBrush(QBrush(Qt::white));
    axisX->setLabelsColor(Qt::white);
    axisY->setRange(rangeY.first, rangeY.second);
    axisY->setLabelsColor(Qt::white);
    chart.addAxis(axisX, Qt::AlignBottom);
    chart.addAxis(axisY, Qt::AlignLeft);
    chart.addSeries(histogram->barSeries);
    histogram->barSeries->attachAxis(axisX);
    histogram->barSeries->attachAxis(axisY);
    chart.setAnimationOptions(QtCharts::QChart::NoAnimation);
    chart.legend()->hide();

    return histogram;
}

std::unique_ptr<LineGraph> AnalyticsModel::createChartWithLineGraph(QtCharts::QChart &chart,
                                                                    QString title,
                                                                    QString axisTitle) {
    chart.setBackgroundBrush(QBrush(CHART_BACKGROUND_COLOR));
    chart.setTitleBrush(QBrush(Qt::white));

    chart.setTitle(title);
    auto axisX = new QtCharts::QValueAxis;
    auto axisY = new QtCharts::QValueAxis;
    auto graph = std::make_unique<LineGraph>(*axisX, *axisY);
    axisX->setRange(0, LINE_HISTORY_SECONDS);
    axisX->setTitleText("Elapsed time (s)");
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setLabelsColor(Qt::white);
    axisY->setRange(-1, 1);
    axisY->setTitleText(axisTitle);
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setLabelsColor(Qt::white);
    chart.addAxis(axisX, Qt::AlignBottom);
    chart.addAxis(axisY, Qt::AlignLeft);
    chart.addSeries(graph->lineSeries);
    graph->lineSeries->attachAxis(axisX);
    graph->lineSeries->attachAxis(axisY);
    chart.setAnimationOptions(QtCharts::QChart::NoAnimation);
    chart.legend()->hide();

    return graph;
}

void AnalyticsModel::updateHistograms(const HomeSettings &settings) {
    const qint64 elapsedMs = m_historyClock.elapsed();
    const auto lights = settings.lights();
    m_analyticsData.histograms.livingRoomLight->update(lights.livingRoomLightOn, elapsedMs);
    m_analyticsData.histograms.bedroomLight->update(lights.bedroomLightOn, elapsedMs);
    m_analyticsData.histograms.kitchenLight->update(lights.kitchenLightOn, elapsedMs);
    m_analyticsData.histograms.acOn->update(settings.ac().on, elapsedMs);
}

void AnalyticsModel::updateLineGraphs(const HomeSettings &settings) {
    // Sample at most once per second; a delayed callback does not invent readings.
    const qint64 elapsedMs = m_historyClock.elapsed();
    if (m_lastLineSampleMs >= 0 && elapsedMs - m_lastLineSampleMs < LINE_SAMPLE_INTERVAL_MS) {
        return;
    }
    m_lastLineSampleMs = elapsedMs;
    const auto readings = settings.sensors();
    m_analyticsData.lineGraphs.temperatureSensor->update(readings.temperature, elapsedMs);
    m_analyticsData.lineGraphs.humiditySensor->update(readings.humidity, elapsedMs);
    m_analyticsData.lineGraphs.brightnessSensor->update(readings.brightness, elapsedMs);
}

void AnalyticsModel::updateAnalyticsData(const HomeSettings &settings) {
    updateHistograms(settings);
    updateLineGraphs(settings);
}
