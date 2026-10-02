#include "lights_panel.h"

#include <QSignalBlocker>

#include "ui_lights_panel.h"
LightsPanel::LightsPanel(QWidget *parent)
    : QWidget(parent), m_ui(std::make_unique<Ui::LightsPanel>()) {
    m_ui->setupUi(this);
    // Custom QWidget subclasses need this to paint their stylesheet background.
    setAttribute(Qt::WA_StyledBackground, true);
    connect(m_ui->livingRoomLightBtn, &QPushButton::toggled, this,
            [this](bool on) { emit lightRequested(smart_home::gui::Light::LIVING_ROOM, on); });
    connect(m_ui->bedroomLightBtn, &QPushButton::toggled, this,
            [this](bool on) { emit lightRequested(smart_home::gui::Light::BEDROOM, on); });
    connect(m_ui->kitchenLightBtn, &QPushButton::toggled, this,
            [this](bool on) { emit lightRequested(smart_home::gui::Light::KITCHEN, on); });
}
LightsPanel::~LightsPanel() = default;
void LightsPanel::render(const LightSettings &snapshot, bool available) {
    const QSignalBlocker livingRoomLightBtnBlocker(m_ui->livingRoomLightBtn);
    m_ui->livingRoomLightBtn->setEnabled(available);
    m_ui->livingRoomLightBtn->setChecked(snapshot.livingRoomLightOn);
    m_ui->livingRoomLightBtn->setIcon(QIcon(snapshot.livingRoomLightOn
                                                ? ":/icons/toggle-on-colored.svg"
                                                : ":/icons/toggle-off-colored.svg"));
    const QSignalBlocker bedroomLightBtnBlocker(m_ui->bedroomLightBtn);
    m_ui->bedroomLightBtn->setEnabled(available);
    m_ui->bedroomLightBtn->setChecked(snapshot.bedroomLightOn);
    m_ui->bedroomLightBtn->setIcon(QIcon(snapshot.bedroomLightOn
                                             ? ":/icons/toggle-on-colored.svg"
                                             : ":/icons/toggle-off-colored.svg"));
    const QSignalBlocker kitchenLightBtnBlocker(m_ui->kitchenLightBtn);
    m_ui->kitchenLightBtn->setEnabled(available);
    m_ui->kitchenLightBtn->setChecked(snapshot.kitchenLightOn);
    m_ui->kitchenLightBtn->setIcon(QIcon(snapshot.kitchenLightOn
                                             ? ":/icons/toggle-on-colored.svg"
                                             : ":/icons/toggle-off-colored.svg"));
}
