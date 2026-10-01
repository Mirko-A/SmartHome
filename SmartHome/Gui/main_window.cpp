#include "main_window.h"

#include <assert.h>

#include <QCloseEvent>
#include <QDateTime>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QToolBar>
#include <nlohmann/json.hpp>

#include "home_settings.h"
#include "ui_main_window.h"

const QVector<QString> PAGE_ICON_PATHS = {
    ":/icons/three-dots-0-purple.svg",
    ":/icons/three-dots-1-purple.svg",
    ":/icons/three-dots-2-purple.svg",
};

const QVector<QVector<QString>> ANALYTICS_PAGE_PATHS = {
    // INACTIVE ICONS
    {
        ":/icons/analytics-lights-off.svg",
        ":/icons/analytics-ac-off.svg",
        ":/icons/analytics-sensors-off.svg",
    },
    // ACTIVE ICONS
    {
        ":/icons/analytics-lights-on.svg",
        ":/icons/analytics-ac-on.svg",
        ":/icons/analytics-sensors-on.svg",
    },
};

constexpr int INITIAL_PLAYER_VOLUME = 50;
// TODO: Set to a lower value for easier testing
// constexpr int ONE_SEC_IN_TICKS = 20;
constexpr int ONE_SEC_IN_TICKS = 2;

MainWindow::MainWindow(std::string configPath, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      m_session(new smart_home::gui::GuiSession(std::move(configPath), this)) {
    ui->setupUi(this);
    connect(ui->devicesBtn, &QAbstractButton::clicked, this, &MainWindow::devicesBtnClicked);
    connect(ui->mediaBtn, &QAbstractButton::clicked, this, &MainWindow::mediaBtnClicked);
    connect(ui->analyticsBtn, &QAbstractButton::clicked, this, &MainWindow::analyticsBtnClicked);
    connect(ui->livingRoomLightBtn, &QAbstractButton::toggled, this,
            &MainWindow::livingRoomLightBtnToggled);
    connect(ui->bedroomLightBtn, &QAbstractButton::toggled, this,
            &MainWindow::bedroomLightBtnToggled);
    connect(ui->kitchenLightBtn, &QAbstractButton::toggled, this,
            &MainWindow::kitchenLightBtnToggled);
    connect(ui->ACOnBtn, &QAbstractButton::toggled, this, &MainWindow::acOnBtnToggled);
    connect(ui->ACModeUp, &QAbstractButton::clicked, this, &MainWindow::acModeUpClicked);
    connect(ui->ACModeDown, &QAbstractButton::clicked, this, &MainWindow::acModeDownClicked);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &MainWindow::volumeSliderValueChanged);
    connect(ui->bassSlider, &QSlider::valueChanged, this, &MainWindow::bassSliderValueChanged);
    connect(ui->pitchSlider, &QSlider::valueChanged, this, &MainWindow::pitchSliderValueChanged);
    connect(ui->analyticsPageLightsBtn, &QAbstractButton::clicked, this,
            &MainWindow::analyticsPageLightsBtnClicked);
    connect(ui->analyticsPageACBtn, &QAbstractButton::clicked, this,
            &MainWindow::analyticsPageACBtnClicked);
    connect(ui->analyticsPageSensorsBtn, &QAbstractButton::clicked, this,
            &MainWindow::analyticsPageSensorsBtnClicked);

    ui->pages->setCurrentIndex(
        static_cast<int>(PageIndex::HOME)); // Set the initial tab to HOME tab

    auto *toolbar = addToolBar("Settings");
    toolbar->setObjectName("settingsToolbar");
    toolbar->setMovable(false);
    m_saveAction = toolbar->addAction("Save settings");
    m_saveAction->setShortcut(QKeySequence::Save);
    m_reloadAction = toolbar->addAction("Reload settings");
    m_configStatus = new QLabel(this);
    m_configStatus->setWordWrap(true);
    m_configStatus->setTextFormat(Qt::PlainText);
    statusBar()->addWidget(m_configStatus, 1);
    connect(m_saveAction, &QAction::triggered, m_session, &smart_home::gui::GuiSession::save);
    connect(m_reloadAction, &QAction::triggered, this, &MainWindow::reloadSettings);
    connect(m_session, &smart_home::gui::GuiSession::changed, this, &MainWindow::refreshSession);
    // There is no AC target temperature in the settings model.
    ui->ACTemperatureUp->setEnabled(false);
    ui->ACTemperatureDown->setEnabled(false);
    ui->ACTemperatureUp->setToolTip("Target temperature is unsupported");
    ui->ACTemperatureDown->setToolTip("Target temperature is unsupported");
    ui->volumeSlider->setRange(0, 100);
    ui->bassSlider->setRange(0, 100);
    ui->pitchSlider->setRange(0, 100);
    refreshSession();
    m_session->reload();

    mediaPlayer = new MediaPlayer;
    mediaPlayer->m_player->setVolume(INITIAL_PLAYER_VOLUME);
    loadMediaPlayerWidgets();

    initAnalyticsModel();
    ui->analyticsPages->setCurrentIndex(static_cast<int>(AnalyticsPageIndex::LIGHT_ANALYTICS));

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::onUpdate);
    updateTimer->start(50);
}

MainWindow::~MainWindow() {
    updateTimer->stop();
    analyticsModel.reset();
    delete mediaPlayer;
    delete ui;

    delete m_session;

    delete updateTimer;
}

void MainWindow::updateLightsUI() {
    ui->livingRoomLightBtn->setChecked(m_session->settings().lights().livingRoomLightOn);
    ui->bedroomLightBtn->setChecked(m_session->settings().lights().bedroomLightOn);
    ui->kitchenLightBtn->setChecked(m_session->settings().lights().kitchenLightOn);
}
void MainWindow::updateSensorsUI() {
    ui->temperatureSensorValueLabel->setText(
        QString::number(m_session->settings().sensors().temperature));
    ui->humiditySensorValueLabel->setText(
        QString::number(m_session->settings().sensors().humidity));
    ui->brightnessSensorValueLabel->setText(
        QString::number(m_session->settings().sensors().brightness));
}
void MainWindow::updateACUI() {
    ui->ACOnBtn->setChecked(m_session->settings().ac().on);
    ui->ACModeValueLabel->setText(
        QString::fromStdString(Ac::modeAsString(m_session->settings().ac().mode)));
}
void MainWindow::updateSpeakersUI() {
    ui->volumeSlider->setValue(m_session->settings().speakers().volume);
    ui->volumeSliderValueLabel->setText(QString::number(m_session->settings().speakers().volume));

    ui->bassSlider->setValue(m_session->settings().speakers().bass);
    ui->bassSliderValueLabel->setText(QString::number(m_session->settings().speakers().bass));

    ui->pitchSlider->setValue(m_session->settings().speakers().pitch);
    ui->pitchSliderValueLabel->setText(QString::number(m_session->settings().speakers().pitch));
}

void MainWindow::updateHomeWidgets() {
    const QSignalBlocker living(ui->livingRoomLightBtn), bedroom(ui->bedroomLightBtn),
        kitchen(ui->kitchenLightBtn), ac(ui->ACOnBtn), volume(ui->volumeSlider),
        bass(ui->bassSlider), pitch(ui->pitchSlider);

    updateLightsUI();
    updateSensorsUI();
    updateACUI();
    updateSpeakersUI();
    for (auto *button :
         {ui->livingRoomLightBtn, ui->bedroomLightBtn, ui->kitchenLightBtn, ui->ACOnBtn}) {
        button->setIcon(QIcon(button->isChecked() ? ":/icons/toggle-on-colored.svg"
                                                  : ":/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::loadMediaPlayerWidgets() {
    connect(mediaPlayer, &MediaPlayer::availabilityChanged, ui->mediaPlayer, &QWidget::setEnabled);
    connect(mediaPlayer, &MediaPlayer::statusTextChanged, ui->mediaStatusLabel, &QLabel::setText);
    connect(mediaPlayer, &MediaPlayer::busyChanged, this, [this](bool busy) {
        if (busy)
            ui->mediaPlayer->setCursor(Qt::BusyCursor);
        else
            ui->mediaPlayer->unsetCursor();
    });
    ui->mediaPlayerVideoWidgetContainer->addWidget(mediaPlayer->m_videoWidget);
    mediaPlayer->m_playlistView = ui->mediaPlaylistListView;
    mediaPlayer->m_labelDuration = ui->mediaPlayerDurationLabel;
    mediaPlayer->m_seekSlider = ui->mediaPlayerSeekSlider;

    loadMediaPlayerControlWidgets();

    mediaPlayer->initUi();
}

void MainWindow::loadMediaPlayerControlWidgets() {
    mediaPlayer->m_controls->m_playButton = ui->mediaPlayerPlayBtn;
    mediaPlayer->m_controls->m_stopButton = ui->mediaPlayerStopBtn;
    mediaPlayer->m_controls->m_nextButton = ui->mediaPlayerNextBtn;
    mediaPlayer->m_controls->m_previousButton = ui->mediaPlayerPrevBtn;
    mediaPlayer->m_controls->m_muteButton = ui->mediaPlayerMuteBtn;
    mediaPlayer->m_controls->m_volumeSlider = ui->mediaPlayerVolumeSlider;

    mediaPlayer->m_playlistModel->m_openButton = ui->mediaPlaylistOpenBtn;
    mediaPlayer->m_playlistModel->m_removeButton = ui->mediaPlaylistRemoveBtn;
}

void MainWindow::initAnalyticsModel() {
    analyticsModel = std::make_unique<AnalyticsModel>(AnalyticsCharts{
        ui->livingRoomLightChartView->chart(),
        ui->bedroomLightChartView->chart(),
        ui->kitchenLightChartView->chart(),
        ui->ACOnChartView->chart(),
        ui->temperatureSensorChartView->chart(),
        ui->humiditySensorChartView->chart(),
        ui->brightnessSensorChartView->chart(),
    });
}

void MainWindow::updateUI() {
    updateDateTimeWidget();
}

void MainWindow::onUpdate() {
    static size_t tickCounter = 0;

    if (m_session->loaded() && (tickCounter % ONE_SEC_IN_TICKS) == 0)
        analyticsModel->updateAnalyticsData(m_session->settings());

    updateUI();

    tickCounter++;
}

void MainWindow::refreshSession() {
    m_configStatus->setText(m_session->status());
    m_saveAction->setEnabled(m_session->loaded() && m_session->dirty() && !m_session->busy());
    m_reloadAction->setEnabled(!m_session->busy());
    const bool editable = m_session->editable();
    for (QWidget *widget : std::initializer_list<QWidget *>{
             ui->livingRoomLightBtn, ui->bedroomLightBtn, ui->kitchenLightBtn, ui->ACOnBtn,
             ui->ACModeUp, ui->ACModeDown}) {
        widget->setEnabled(editable);
    }
    ui->volumeSlider->setEnabled(editable);
    ui->bassSlider->setEnabled(editable);
    ui->pitchSlider->setEnabled(editable);
    if (m_session->loaded()) {
        updateHomeWidgets();
    } else {
        ui->temperatureSensorValueLabel->setText("—");
        ui->humiditySensorValueLabel->setText("—");
        ui->brightnessSensorValueLabel->setText("—");
    }
}

void MainWindow::editControls() {
    if (!m_session->editable())
        return;
    auto settings = m_session->settings();
    settings.setLights(ui->livingRoomLightBtn->isChecked(), ui->bedroomLightBtn->isChecked(),
                       ui->kitchenLightBtn->isChecked());
    settings.setAc(ui->ACOnBtn->isChecked(), settings.ac().mode);
    if (settings.setSpeakers(ui->volumeSlider->value(), ui->bassSlider->value(),
                             ui->pitchSlider->value())) {
        m_session->edit(settings);
    }
}

void MainWindow::reloadSettings() {
    if (m_session->dirty() &&
        QMessageBox::question(
            this, "Reload settings", "Discard pending edits if the configuration reload succeeds?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;
    m_session->reload();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (m_session->busy()) {
        statusBar()->showMessage("Wait for the configuration operation to finish before closing.",
                                 4000);
        event->ignore();
        return;
    }
    if (m_session->dirty() &&
        QMessageBox::question(
            this, "Unsaved settings",
            "Discard pending edits and close? Use Save settings first to keep them.",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) {
        event->ignore();
        return;
    }
    event->accept();
}

void MainWindow::updateCurrentPage(PageIndex index) {
    ui->pages->setCurrentIndex(static_cast<int>(index)); // reveal the home page
    ui->buttonsCurrentButton->setIcon(
        QIcon(PAGE_ICON_PATHS.at(static_cast<int>(index)))); // update current page icon
}

void MainWindow::updateDateTimeWidget() {
    QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
    ui->dateTimeLabel->setText(currentDateTime.toString());
}

void MainWindow::devicesBtnClicked() {
    updateCurrentPage(PageIndex::HOME);
}

void MainWindow::mediaBtnClicked() {
    updateCurrentPage(PageIndex::MEDIA);
}

void MainWindow::analyticsBtnClicked() {
    updateCurrentPage(PageIndex::ANALYTICS);
}

void MainWindow::livingRoomLightBtnToggled(bool) {
    editControls();
}
void MainWindow::bedroomLightBtnToggled(bool) {
    editControls();
}
void MainWindow::kitchenLightBtnToggled(bool) {
    editControls();
}
void MainWindow::acOnBtnToggled(bool) {
    editControls();
}

void MainWindow::acModeUpClicked() {
    auto settings = m_session->settings();
    const int mode = static_cast<int>(settings.ac().mode);
    if (mode < static_cast<int>(Ac::Mode::TURBO)) {
        settings.setAc(settings.ac().on, static_cast<Ac::Mode>(mode + 1));
        m_session->edit(settings);
    }
}

void MainWindow::acModeDownClicked() {
    auto settings = m_session->settings();
    const int mode = static_cast<int>(settings.ac().mode);
    if (mode > static_cast<int>(Ac::Mode::NORMAL)) {
        settings.setAc(settings.ac().on, static_cast<Ac::Mode>(mode - 1));
        m_session->edit(settings);
    }
}

void MainWindow::volumeSliderValueChanged(int) {
    editControls();
}
void MainWindow::bassSliderValueChanged(int) {
    editControls();
}
void MainWindow::pitchSliderValueChanged(int) {
    editControls();
}

void MainWindow::updateAnalyticsPageIcon(AnalyticsPageIndex pageIndex,
                                         AnalyticsPageState newState) {
    switch (pageIndex) {
    case AnalyticsPageIndex::LIGHT_ANALYTICS: {
        ui->analyticsPageLightsBtn->setIcon(
            QIcon(ANALYTICS_PAGE_PATHS.at(static_cast<int>(newState))
                      .at(static_cast<int>(AnalyticsPageIndex::LIGHT_ANALYTICS))));
    } break;
    case AnalyticsPageIndex::AC_ANALYTICS: {
        ui->analyticsPageACBtn->setIcon(
            QIcon(ANALYTICS_PAGE_PATHS.at(static_cast<int>(newState))
                      .at(static_cast<int>(AnalyticsPageIndex::AC_ANALYTICS))));
    } break;
    case AnalyticsPageIndex::SENSORS_ANALYTICS: {
        ui->analyticsPageSensorsBtn->setIcon(
            QIcon(ANALYTICS_PAGE_PATHS.at(static_cast<int>(newState))
                      .at(static_cast<int>(AnalyticsPageIndex::SENSORS_ANALYTICS))));
    } break;
    default:
        assert(false && "Invalid analytics page index provided!");
        break;
    }
}

void MainWindow::deselectCurrentAnalyticsPage() {
    updateAnalyticsPageIcon(static_cast<AnalyticsPageIndex>(ui->analyticsPages->currentIndex()),
                            AnalyticsPageState::OFF);
}
void MainWindow::selectNewAnalyticsPage(AnalyticsPageIndex newPageIndex) {
    updateAnalyticsPageIcon(newPageIndex, AnalyticsPageState::ON);

    ui->analyticsPages->setCurrentIndex(static_cast<int>(newPageIndex));
}

void MainWindow::swapSelectedAnalyticsPage(AnalyticsPageIndex newPageIndex) {
    if (static_cast<int>(newPageIndex) != ui->analyticsPages->currentIndex()) {
        deselectCurrentAnalyticsPage();
        selectNewAnalyticsPage(newPageIndex);
    }
}

void MainWindow::analyticsPageLightsBtnClicked() {
    swapSelectedAnalyticsPage(AnalyticsPageIndex::LIGHT_ANALYTICS);
}

void MainWindow::analyticsPageACBtnClicked() {
    swapSelectedAnalyticsPage(AnalyticsPageIndex::AC_ANALYTICS);
}

void MainWindow::analyticsPageSensorsBtnClicked() {
    swapSelectedAnalyticsPage(AnalyticsPageIndex::SENSORS_ANALYTICS);
}
