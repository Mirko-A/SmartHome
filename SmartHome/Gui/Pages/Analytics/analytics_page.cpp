#include "analytics_page.h"

#include "gui_app.h"
#include "ui_analytics_page.h"

using namespace smart_home;

AnalyticsPage::AnalyticsPage(gui::App &app, QWidget *parent)
    : QWidget(parent), m_app(app), m_ui(std::make_unique<Ui::AnalyticsPage>()) {
    m_ui->setupUi(this);

    // Histograms.
    m_histograms[0] =
        std::make_unique<Histogram>(*m_ui->livingRoomLightChartView->chart(), "Living room");
    m_histograms[1] = std::make_unique<Histogram>(*m_ui->bedroomLightChartView->chart(), "Bedroom");
    m_histograms[2] = std::make_unique<Histogram>(*m_ui->kitchenLightChartView->chart(), "Kitchen");
    m_histograms[3] = std::make_unique<Histogram>(*m_ui->ACOnChartView->chart(), "AC");

    // Line graphs.
    m_graphs[0] = std::make_unique<LineGraph>(*m_ui->temperatureSensorChartView->chart(),
                                              "Temperature", &SensorReadings::temperature);
    m_graphs[1] = std::make_unique<LineGraph>(*m_ui->humiditySensorChartView->chart(), "Humidity",
                                              &SensorReadings::humidity);
    m_graphs[2] = std::make_unique<LineGraph>(*m_ui->brightnessSensorChartView->chart(),
                                              "Brightness", &SensorReadings::brightness);

    // Charts.
    m_ui->ACTemperatureChartView->chart()->setTitle("Target temperature is unsupported");

    // Qt.
    connect(m_ui->analyticsPageLightsBtn, &QPushButton::clicked, this, [this] { selectPage(0); });
    connect(m_ui->analyticsPageACBtn, &QPushButton::clicked, this, [this] { selectPage(1); });
    connect(m_ui->analyticsPageSensorsBtn, &QPushButton::clicked, this, [this] { selectPage(2); });
    connect(&app, &gui::App::tick, this, &AnalyticsPage::render);
    connect(&app, &gui::App::changed, this, &AnalyticsPage::render);

    selectPage(0);
    render();
}

AnalyticsPage::~AnalyticsPage() = default;

void AnalyticsPage::selectPage(int index) {
    const std::array<QString, 3> names{"lights", "ac", "sensors"};
    const std::array<QPushButton *, 3> buttons{
        m_ui->analyticsPageLightsBtn, m_ui->analyticsPageACBtn, m_ui->analyticsPageSensorsBtn};

    for (int i = 0; i < 3; ++i) {
        auto fName = ":/icons/analytics-" + names[i] + (i == index ? "-on.svg" : "-off.svg");
        buttons[i]->setIcon(QIcon(fName));
    }
    m_ui->analyticsPages->setCurrentIndex(index);
}

void AnalyticsPage::render() {
    const auto &model = m_app.analytics();
    for (size_t i = 0; i < m_histograms.size(); ++i) {
        m_histograms[i]->render(model.onTime()[i]);
    }
    for (auto &graph : m_graphs) {
        graph->render(model.samples());
    }
}
