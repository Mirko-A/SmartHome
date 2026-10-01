#include "devices_page.h"

#include "gui_app.h"
#include "ui_devices_page.h"
using smart_home::gui::GuiApp;
DevicesPage::DevicesPage(GuiApp &app, QWidget *parent)
    : QWidget(parent), m_app(app), m_ui(std::make_unique<Ui::DevicesPage>()) {
    m_ui->setupUi(this);
    connect(m_ui->lightsContainer, &LightsPanel::lightRequested, &app, &GuiApp::setLight);
    connect(m_ui->ACContainer, &AcPanel::onRequested, &app, &GuiApp::setAcOn);
    connect(m_ui->ACContainer, &AcPanel::modeStepRequested, &app, &GuiApp::stepAcMode);
    connect(m_ui->speakersContainer, &SpeakersPanel::valueRequested, &app, &GuiApp::setSpeaker);
    connect(&app, &GuiApp::changed, this, &DevicesPage::render);
    render();
}
DevicesPage::~DevicesPage() = default;
void DevicesPage::render() {
    const auto &state = m_app.state();
    m_ui->lightsContainer->render(state.pending.lights(), m_app.editable());
    m_ui->ACContainer->render(state.pending.ac(), m_app.editable());
    m_ui->speakersContainer->render(state.pending.speakers(), m_app.editable());
    m_ui->sensorsContainer->render(state.pending.sensors(), state.loaded);
}
