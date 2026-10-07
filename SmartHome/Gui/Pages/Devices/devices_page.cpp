#include "devices_page.h"

#include "gui_app.h"
#include "ui_devices_page.h"

using namespace smart_home;

DevicesPage::DevicesPage(gui::App &app, QWidget *parent)
    : QWidget(parent), m_app(app), m_ui(std::make_unique<Ui::DevicesPage>()) {
    m_ui->setupUi(this);

    connect(m_ui->lightsContainer, &LightsPanel::lightRequested, &app, &gui::App::setLight);
    connect(m_ui->ACContainer, &AcPanel::onRequested, &app, &gui::App::setAcOn);
    connect(m_ui->ACContainer, &AcPanel::modeStepRequested, &app, &gui::App::stepAcMode);
    connect(m_ui->speakersContainer, &SpeakersPanel::valueRequested, &app, &gui::App::setSpeaker);
    connect(&app, &gui::App::changed, this, &DevicesPage::render);

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
