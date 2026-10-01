#include "ac_panel.h"

#include <QSignalBlocker>

#include "ui_ac_panel.h"
AcPanel::AcPanel(QWidget *parent) : QWidget(parent), m_ui(std::make_unique<Ui::AcPanel>()) {
    m_ui->setupUi(this);
    m_ui->ACTemperatureUp->setEnabled(false);
    m_ui->ACTemperatureDown->setEnabled(false);
    m_ui->ACTemperatureUp->setToolTip("Target temperature is unsupported");
    m_ui->ACTemperatureDown->setToolTip("Target temperature is unsupported");
    m_ui->ACTemperatureValueLabel->setText("—");
    connect(m_ui->ACOnBtn, &QPushButton::toggled, this, &AcPanel::onRequested);
    connect(m_ui->ACModeUp, &QPushButton::clicked, this, [this] { emit modeStepRequested(1); });
    connect(m_ui->ACModeDown, &QPushButton::clicked, this, [this] { emit modeStepRequested(-1); });
}
AcPanel::~AcPanel() = default;
void AcPanel::render(const AcSettings &snapshot, bool available) {
    const QSignalBlocker blocker(m_ui->ACOnBtn);
    m_ui->ACOnBtn->setEnabled(available);
    m_ui->ACModeUp->setEnabled(available);
    m_ui->ACModeDown->setEnabled(available);
    m_ui->ACOnBtn->setChecked(snapshot.on);
    m_ui->ACOnBtn->setIcon(
        QIcon(snapshot.on ? ":/icons/toggle-on-colored.svg" : ":/icons/toggle-off-colored.svg"));
    m_ui->ACModeValueLabel->setText(QString::fromStdString(Ac::modeAsString(snapshot.mode)));
}
