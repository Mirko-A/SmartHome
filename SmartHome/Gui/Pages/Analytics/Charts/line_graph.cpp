#include "line_graph.h"

#include <algorithm>

LineGraph::LineGraph(QtCharts::QChart &chart, const QString &title,
                     int16_t SensorReadings::*reading)
    : m_series(new QtCharts::QLineSeries), m_x(new QtCharts::QValueAxis),
      m_y(new QtCharts::QValueAxis), m_reading(reading) {
    // Title & style.
    chart.setTitle(title);
    chart.setTitleBrush(Qt::white);
    chart.setBackgroundBrush(QColor(52, 59, 71));
    QPen pen(QColor(160, 110, 181));
    pen.setWidth(3);
    m_series->setPen(pen);

    // Construct the graph.
    m_x->setRange(0, 3600);
    m_x->setTitleText("Elapsed time (s)");
    m_y->setRange(-1, 1);
    m_y->setTitleText(title + " (unit unspecified)");
    for (auto *axis : {m_x, m_y}) {
        axis->setTitleBrush(Qt::white);
        axis->setLabelsColor(Qt::white);
    }
    chart.addAxis(m_x, Qt::AlignBottom);
    chart.addAxis(m_y, Qt::AlignLeft);

    chart.addSeries(m_series);
    m_series->attachAxis(m_x);
    m_series->attachAxis(m_y);

    chart.legend()->hide();
}

void LineGraph::render(const std::deque<SensorSample> &samples) {
    if (samples.empty() || samples.back().seconds == m_lastSample) {
        return;
    }
    m_lastSample = samples.back().seconds;

    QVector<QPointF> points;
    points.reserve(static_cast<int>(samples.size()));
    double minimum = samples.front().readings.*m_reading;
    double maximum = minimum;
    for (const auto &sample : samples) {
        const double value = sample.readings.*m_reading;
        points.append(QPointF(sample.seconds, value));
        minimum = std::min(minimum, value);
        maximum = std::max(maximum, value);
    }
    m_series->replace(points);

    const double padding = std::max(1.0, (maximum - minimum) * 0.05);
    m_x->setRange(std::max(0.0, m_lastSample - 3600), std::max(3600.0, m_lastSample));
    m_y->setRange(minimum - padding, maximum + padding);
}
