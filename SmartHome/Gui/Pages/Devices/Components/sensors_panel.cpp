#include "sensors_panel.h"

#include <QSignalBlocker>

#include "ui_sensors_panel.h"

SensorsPanel::SensorsPanel(QWidget *parent)
    : QWidget(parent), m_ui(std::make_unique<Ui::SensorsPanel>()) {
    m_ui->setupUi(this);

    // Custom QWidget subclasses need this to paint their stylesheet background.
    setAttribute(Qt::WA_StyledBackground, true);
}

SensorsPanel::~SensorsPanel() = default;

void SensorsPanel::render(const SensorReadings &snapshot) {
    m_ui->temperatureSensorValueLabel->setText(QString::number(snapshot.temperature));
    m_ui->humiditySensorValueLabel->setText(QString::number(snapshot.humidity));
    m_ui->brightnessSensorValueLabel->setText(QString::number(snapshot.brightness));
}
