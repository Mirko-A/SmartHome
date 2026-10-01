#include "histogram.h"

#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QValueAxis>
Histogram::Histogram(QtCharts::QChart &chart, const QString &title)
    : m_bars(new QtCharts::QBarSet(title)) {
    chart.setTitle(title);
    auto font = chart.titleFont();
    font.setPointSize(16);
    chart.setTitleFont(font);
    chart.setTitleBrush(Qt::white);
    chart.setBackgroundBrush(QColor(52, 59, 71));
    m_bars->setColor(QColor(160, 110, 181));
    auto *series = new QtCharts::QBarSeries;
    series->append(m_bars);
    series->setBarWidth(1);
    auto *x = new QtCharts::QBarCategoryAxis;
    QStringList categories;
    for (int i = 1; i <= 24; ++i)
        categories.append(QString::number(i));
    x->append(categories);
    x->setTitleText("Hourly buckets (oldest to newest)");
    x->setTitleBrush(Qt::white);
    x->setLabelsColor(Qt::white);
    auto *y = new QtCharts::QValueAxis;
    y->setRange(0, 60);
    y->setTitleText("Requested on-time (min)");
    y->setTitleBrush(Qt::white);
    y->setLabelsColor(Qt::white);
    chart.addAxis(x, Qt::AlignBottom);
    chart.addAxis(y, Qt::AlignLeft);
    chart.addSeries(series);
    series->attachAxis(x);
    series->attachAxis(y);
    chart.setAnimationOptions(QtCharts::QChart::NoAnimation);
    chart.legend()->hide();
}
void Histogram::render(const OnTimeHistory &history) {
    if (m_bars->count() != static_cast<int>(history.minutes.size())) {
        m_bars->remove(0, m_bars->count());
        for (auto value : history.minutes)
            *m_bars << value;
    } else {
        for (size_t i = 0; i < history.minutes.size(); ++i)
            m_bars->replace(static_cast<int>(i), history.minutes[i]);
    }
}
