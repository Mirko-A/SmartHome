#include "main_window.h"

#include <assert.h>

#include <QDateTime>
#include <QFileDialog>
#include <nlohmann/json.hpp>

#include "home_settings.h"
#include "ui_main_window.h"

#define CFG_JSON_FILE_PATH CFG_JSON_FILE_PATH_QSTR.toStdString().c_str()
#define INI_JSON_FILE_PATH INI_JSON_FILE_PATH_QSTR.toStdString().c_str()

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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->pages->setCurrentIndex(
        static_cast<int>(PageIndex::HOME)); // Set the initial tab to HOME tab

    homeSettings = new HomeSettings;
    loadHomeCfgWidgets();

    mediaPlayer = new MediaPlayer;
    loadMediaPlayerWidgets();
    mediaPlayer->m_player->setVolume(INITIAL_PLAYER_VOLUME);

    initAnalyticsModel();
    ui->analyticsPages->setCurrentIndex(static_cast<int>(AnalyticsPageIndex::LIGHT_ANALYTICS));

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    updateTimer->start(50);
}

MainWindow::~MainWindow() {
    updateTimer->stop();
    analyticsModel.reset();
    delete mediaPlayer;
    delete ui;

    delete homeSettings;

    delete updateTimer;
}

void MainWindow::updateLightsUI() {
    ui->livingRoomLightBtn->setChecked(homeSettings->lights().livingRoomLightOn);
    ui->bedroomLightBtn->setChecked(homeSettings->lights().bedroomLightOn);
    ui->kitchenLightBtn->setChecked(homeSettings->lights().kitchenLightOn);
}
void MainWindow::updateSensorsUI() {
    ui->temperatureSensorValueLabel->setText(QString::number(homeSettings->sensors().temperature));
    ui->humiditySensorValueLabel->setText(QString::number(homeSettings->sensors().humidity));
    ui->brightnessSensorValueLabel->setText(QString::number(homeSettings->sensors().brightness));
}
void MainWindow::updateACUI() {
    ui->ACOnBtn->setChecked(homeSettings->ac().on);
    ui->ACModeValueLabel->setText(
        QString::fromStdString(Ac::modeAsString(homeSettings->ac().mode)));
}
void MainWindow::updateSpeakersUI() {
    ui->volumeSlider->setValue(homeSettings->speakers().volume);
    ui->volumeSliderValueLabel->setText(QString::number(homeSettings->speakers().volume));

    ui->bassSlider->setValue(homeSettings->speakers().bass);
    ui->bassSliderValueLabel->setText(QString::number(homeSettings->speakers().bass));

    ui->pitchSlider->setValue(homeSettings->speakers().pitch);
    ui->pitchSliderValueLabel->setText(QString::number(homeSettings->speakers().pitch));
}

void MainWindow::updateHomeWidgets() {
    updateLightsUI();
    updateSensorsUI();
    updateACUI();
    updateSpeakersUI();
}

void MainWindow::loadMediaPlayerWidgets() {
    ui->mediaPlayerVideoWidgetContainer->addWidget(mediaPlayer->m_videoWidget);
    mediaPlayer->m_playlistView = ui->mediaPlaylistListView;
    mediaPlayer->m_labelDuration = ui->mediaPlayerDurationLabel;
    mediaPlayer->m_seekSlider = ui->mediaPlayerSeekSlider;

    loadMediaPlayerControlWidgets();

    mediaPlayer->initializeUIElements();
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

    // Config has been updated by 3rd party (python script)
    reloadHomeWidgetsIfDirty();
}

void MainWindow::onUpdate() {
    static size_t tickCounter = 0;

    if ((tickCounter % ONE_SEC_IN_TICKS) == 0)
        analyticsModel->updateAnalyticsData(*homeSettings);

    // FIXME: This doesn't exist anymore.
    // homeSettings->onUpdate();
    updateUI();

    // TODO: JSON file handling
    saveHomeSettings();

    tickCounter++;
}

void MainWindow::saveHomeSettings() {
    // FIXME:
    // std::ofstream o(CFG_JSON_FILE_PATH);
    // o << std::setw(4) << homeSettings->serializeJson() << std::endl;
}

nlohmann::json MainWindow::loadHomeSettings() {
    // FIXME:
    // std::ifstream i;
    // i.open(CFG_JSON_FILE_PATH);
    // if (!i.good()) {
    //     i.open(INI_JSON_FILE_PATH);
    // }
    //
    nlohmann::json json;
    // i >> json;
    return json;
}

void MainWindow::loadHomeCfgWidgets() {
    // FIXME:
    // homeSettings->deserializeJson(loadHomeSettings());
    // updateHomeWidgets();
}

void MainWindow::reloadHomeWidgetsIfDirty() {
    nlohmann::json json = loadHomeSettings();
    // homeSettings->loadDirtyFlag(json);
    // FIXME:
    // if (homeSettings->isDirty) {
    //     homeSettings->deserializeJson(json);
    //     updateHomeWidgets();
    //     homeSettings->isDirty = false;
    // }
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

void MainWindow::on_devicesBtn_clicked() {
    updateCurrentPage(PageIndex::HOME);
}

void MainWindow::on_mediaBtn_clicked() {
    updateCurrentPage(PageIndex::MEDIA);
}

void MainWindow::on_analyticsBtn_clicked() {
    updateCurrentPage(PageIndex::ANALYTICS);
}

void MainWindow::on_livingRoomLightBtn_toggled(bool checked) {
    // TODO: handle actual light
    // homeSettings->lights().livingRoomLightOn = checked;

    if (checked) {
        ui->livingRoomLightBtn->setIcon(QIcon(":/icons/toggle-on-colored.svg"));
    } else {
        ui->livingRoomLightBtn->setIcon(QIcon(":/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_bedroomLightBtn_toggled(bool checked) {
    // TODO: handle actual light
    // homeSettings->lights.bedroomLightOn = checked;

    if (checked) {
        ui->bedroomLightBtn->setIcon(QIcon(":/icons/toggle-on-colored.svg"));
    } else {
        ui->bedroomLightBtn->setIcon(QIcon(":/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_kitchenLightBtn_toggled(bool checked) {
    // TODO: handle actual light
    // homeSettings->lights.kitchenLightOn = checked;

    if (checked) {
        ui->kitchenLightBtn->setIcon(QIcon(":/icons/toggle-on-colored.svg"));
    } else {
        ui->kitchenLightBtn->setIcon(QIcon(":/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_ACOnBtn_toggled(bool checked) {
    // TODO: handle actual light
    // homeSettings->AC.on = checked;

    if (checked) {
        ui->ACOnBtn->setIcon(QIcon(":/icons/toggle-on-colored.svg"));
    } else {
        ui->ACOnBtn->setIcon(QIcon(":/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_ACTemperatureUp_clicked() {
    // FIXME: AC has no temp anymore
}

void MainWindow::on_ACTemperatureDown_clicked() {
    // FIXME: AC has no temp anymore
}

void MainWindow::on_ACModeUp_clicked() {
    // FIXME:
    // Ac::Mode currentMode = homeSettings->ac().mode;
    // if (currentMode != 0) {
    //     currentMode++;
    // }

    // homeSettings->AC.mode = static_cast<ACMode>(currentMode);
    // ui->ACModeValueLabel->setText(QString::fromStdString(ACModeToString(homeSettings->AC.mode)));
}

void MainWindow::on_ACModeDown_clicked() {
    // FIXME:
    // uint8_t currentMode = static_cast<uint8_t>(homeSettings->AC.mode);
    // if (currentMode > 0) {
    //     currentMode--;
    // }
    //
    // homeSettings->AC.mode = static_cast<ACMode>(currentMode);
    // ui->ACModeValueLabel->setText(QString::fromStdString(ACModeToString(homeSettings->AC.mode)));
}

void MainWindow::on_volumeSlider_sliderMoved(int position) {
    // FIXME:
    // homeSettings->speakers.volume = position;
    // ui->volumeSliderValueLabel->setText(QString::number(position));
}

void MainWindow::on_volumeSlider_valueChanged(int value) {
    // FIXME:
    // homeSettings->speakers.volume = value;
    // ui->volumeSliderValueLabel->setText(QString::number(value));
}

void MainWindow::on_bassSlider_sliderMoved(int position) {
    // FIXME:
    // homeSettings->speakers.bass = position;
    // ui->bassSliderValueLabel->setText(QString::number(position));
}

void MainWindow::on_bassSlider_valueChanged(int value) {
    // FIXME:
    // homeSettings->speakers.bass = value;
    // ui->bassSliderValueLabel->setText(QString::number(value));
}

void MainWindow::on_pitchSlider_sliderMoved(int position) {
    // FIXME:
    // homeSettings->speakers.pitch = position;
    // ui->pitchSliderValueLabel->setText(QString::number(position));
}

void MainWindow::on_pitchSlider_valueChanged(int value) {
    // FIXME:
    // homeSettings->speakers.pitch = value;
    // ui->pitchSliderValueLabel->setText(QString::number(value));
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

void MainWindow::on_analyticsPageLightsBtn_clicked() {
    swapSelectedAnalyticsPage(AnalyticsPageIndex::LIGHT_ANALYTICS);
}

void MainWindow::on_analyticsPageACBtn_clicked() {
    swapSelectedAnalyticsPage(AnalyticsPageIndex::AC_ANALYTICS);
}

void MainWindow::on_analyticsPageSensorsBtn_clicked() {
    swapSelectedAnalyticsPage(AnalyticsPageIndex::SENSORS_ANALYTICS);
}
