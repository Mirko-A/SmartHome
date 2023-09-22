#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QFileDialog>

#include <fstream>

#define CFG_JSON_FILE_PATH CFG_JSON_FILE_PATH_QSTR.toStdString().c_str()
#define INI_JSON_FILE_PATH INI_JSON_FILE_PATH_QSTR.toStdString().c_str()

const QVector<QString> PAGE_ICON_PATHS =
{
  "../resource/icons/three-dots-0-purple.svg",
  "../resource/icons/three-dots-1-purple.svg",
  "../resource/icons/three-dots-2-purple.svg",
};

const QString CFG_JSON_FILE_PATH_QSTR = "../resource/home_cfg.json";
const QString INI_JSON_FILE_PATH_QSTR = "../resource/ini_cfg.json";

constexpr int INITIAL_PLAYER_VOLUME = 50;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pages->setCurrentIndex(static_cast<int>(PageIndex::HOME)); // Set the initial tab to HOME tab

    homeCfg = new HomeConfig;
    loadHomeCfgWidgets();

    mediaPlayer = new MediaPlayer;
    loadMediaPlayerWidgets();
    mediaPlayer->m_player->setVolume(INITIAL_PLAYER_VOLUME);

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    updateTimer->start(50);
}

MainWindow::~MainWindow()
{
    delete ui;

    delete homeCfg;
    delete mediaPlayer;

    delete updateTimer;
}

void MainWindow::updateLightsUI()
{
    ui->livingRoomLightBtn->setChecked(homeCfg->lights.livingRoomLightOn);
    ui->bedroomLightBtn->setChecked(homeCfg->lights.bedroomLightOn);
    ui->kitchenLightBtn->setChecked(homeCfg->lights.kitchenLightOn);
}
void MainWindow::updateSensorsUI()
{
    ui->temperatureSensorValueLabel->setText(QString::number(homeCfg->sensors.temperature));
    ui->humiditySensorValueLabel->setText(QString::number(homeCfg->sensors.humidity));
    ui->brightnessSensorValueLabel->setText(QString::number(homeCfg->sensors.brightness));
}
void MainWindow::updateACUI()
{
    ui->ACOnBtn->setChecked(homeCfg->AC.on);
    ui->ACTemperatureValueLabel->setText(QString::number(homeCfg->AC.temperature));
    ui->ACModeValueLabel->setText(ACModeToString(homeCfg->AC.mode));
}
void MainWindow::updateSpeakersUI()
{
    ui->volumeSlider->setValue(homeCfg->speakers.volume);
    ui->volumeSliderValueLabel->setText(QString::number(homeCfg->speakers.volume));

    ui->bassSlider->setValue(homeCfg->speakers.bass);
    ui->bassSliderValueLabel->setText(QString::number(homeCfg->speakers.bass));

    ui->pitchSlider->setValue(homeCfg->speakers.pitch);
    ui->pitchSliderValueLabel->setText(QString::number(homeCfg->speakers.pitch));
}

void MainWindow::updateHomeCfgWidgets()
{
    updateLightsUI();
    updateSensorsUI();
    updateACUI();
    updateSpeakersUI();
}

void MainWindow::loadMediaPlayerWidgets()
{
     ui->mediaPlayerVideoWidgetContainer->addWidget(mediaPlayer->m_videoWidget);
     mediaPlayer->m_playlistView = ui->mediaPlaylistListView;
     mediaPlayer->m_labelDuration = ui->mediaPlayerDurationLabel;
     mediaPlayer->m_seekSlider = ui->mediaPlayerSeekSlider;

     loadMediaPlayerControlWidgets();

     mediaPlayer->initializeUIElements();
     mediaPlayer->m_controls->initializeUIElements();
}

void MainWindow::loadMediaPlayerControlWidgets()
{
    mediaPlayer->m_controls->m_playButton = ui->mediaPlayerPlayBtn;
    mediaPlayer->m_controls->m_stopButton = ui->mediaPlayerStopBtn;
    mediaPlayer->m_controls->m_nextButton = ui->mediaPlayerNextBtn;
    mediaPlayer->m_controls->m_previousButton = ui->mediaPlayerPrevBtn;
    mediaPlayer->m_controls->m_muteButton = ui->mediaPlayerMuteBtn;
    mediaPlayer->m_controls->m_volumeSlider = ui->mediaPlayerVolumeSlider;

    mediaPlayer->m_playlistModel->m_openButton = ui->mediaPlaylistOpenBtn;
    mediaPlayer->m_playlistModel->m_removeButton = ui->mediaPlaylistRemoveBtn;
}

void MainWindow::updateUI()
{
    updateDateTimeWidget();

    // Config has been updated by 3rd party (python script)
    reloadHomeCfgWidgetsIfDirty();
}

void MainWindow::onUpdate()
{
    homeCfg->onUpdate();
    updateUI();

    // TODO: JSON file handling
    saveHomeCfgAsJSON();
}

void MainWindow::saveHomeCfgAsJSON()
{
    std::ofstream o(CFG_JSON_FILE_PATH);
    o << std::setw(4) << homeCfg->toJSON() << std::endl;
}

nlohmann::json MainWindow::loadHomeCfgAsJson()
{
     std::ifstream i;
     i.open(CFG_JSON_FILE_PATH);

     if (!i.good())
         i.open(INI_JSON_FILE_PATH);

     nlohmann::json j;
     i >> j;

     return j;
}

void MainWindow::loadHomeCfgWidgets()
{
    nlohmann::json j = loadHomeCfgAsJson();
    homeCfg->fromJSON(j);
    updateHomeCfgWidgets();
}

void MainWindow::reloadHomeCfgWidgetsIfDirty()
{
    nlohmann::json j = loadHomeCfgAsJson();

    homeCfg->loadDirtyFlag(j);
    if (homeCfg->isDirty)
    {
        homeCfg->fromJSON(j);
        updateHomeCfgWidgets();
        homeCfg->isDirty = false;
    }
}

void MainWindow::updateCurrentPage(PageIndex index)
{
    ui->pages->setCurrentIndex(static_cast<int>(index)); // reveal the home page
    ui->buttonsCurrentButton->setIcon(QIcon(PAGE_ICON_PATHS.at(static_cast<int>(index)))); // update current page icon
}

void MainWindow::updateDateTimeWidget()
{
    QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
    ui->dateTimeLabel->setText(currentDateTime.toString());
}

void MainWindow::on_devicesBtn_clicked()
{
    updateCurrentPage(PageIndex::HOME);
}

void MainWindow::on_mediaBtn_clicked()
{
    updateCurrentPage(PageIndex::MEDIA);
}

void MainWindow::on_analyticsBtn_clicked()
{
    updateCurrentPage(PageIndex::ANALYTICS);
}

void MainWindow::on_livingRoomLightBtn_toggled(bool checked)
{
    // TODO: handle actual light
    homeCfg->lights.livingRoomLightOn = checked;

    if (checked)
    {
        ui->livingRoomLightBtn->setIcon(QIcon("../resource/icons/toggle-on-colored.svg"));
    }
    else
    {
        ui->livingRoomLightBtn->setIcon(QIcon("../resource/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_bedroomLightBtn_toggled(bool checked)
{
    // TODO: handle actual light
    homeCfg->lights.bedroomLightOn = checked;

    if (checked)
    {
        ui->bedroomLightBtn->setIcon(QIcon("../resource/icons/toggle-on-colored.svg"));
    }
    else
    {
        ui->bedroomLightBtn->setIcon(QIcon("../resource/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_kitchenLightBtn_toggled(bool checked)
{
    // TODO: handle actual light
    homeCfg->lights.kitchenLightOn = checked;

    if (checked)
    {
        ui->kitchenLightBtn->setIcon(QIcon("../resource/icons/toggle-on-colored.svg"));
    }
    else
    {
        ui->kitchenLightBtn->setIcon(QIcon("../resource/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_ACOnBtn_toggled(bool checked)
{
    // TODO: handle actual light
    homeCfg->AC.on = checked;

    if (checked)
    {
        ui->ACOnBtn->setIcon(QIcon("../resource/icons/toggle-on-colored.svg"));
    }
    else
    {
        ui->ACOnBtn->setIcon(QIcon("../resource/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_ACTemperatureUp_clicked()
{
    // TODO: handle actual light
    homeCfg->AC.temperature += AC_TEMP_STEP;

    if (homeCfg->AC.temperature > MAX_AC_TEMP)
        homeCfg->AC.temperature = MAX_AC_TEMP;

    ui->ACTemperatureValueLabel->setText(QString::number(homeCfg->AC.temperature));
}

void MainWindow::on_ACTemperatureDown_clicked()
{
    // TODO: handle actual light
    homeCfg->AC.temperature -= AC_TEMP_STEP;

    if (homeCfg->AC.temperature < MIN_AC_TEMP)
        homeCfg->AC.temperature = MIN_AC_TEMP;

    ui->ACTemperatureValueLabel->setText(QString::number(homeCfg->AC.temperature));
}

void MainWindow::on_ACModeUp_clicked()
{
    uint8_t currentMode = static_cast<uint8_t>(homeCfg->AC.mode);

    uint8_t modeCnt = static_cast<uint8_t>(ACMode::AC_MODE_CNT);

    if (currentMode < (modeCnt - 1))
    {
        currentMode++;
    }

    homeCfg->AC.mode = static_cast<ACMode>(currentMode);
    ui->ACModeValueLabel->setText(ACModeToString(homeCfg->AC.mode));
}

void MainWindow::on_ACModeDown_clicked()
{
    uint8_t currentMode = static_cast<uint8_t>(homeCfg->AC.mode);

    if (currentMode > 0)
    {
        currentMode--;
    }

    homeCfg->AC.mode = static_cast<ACMode>(currentMode);
    ui->ACModeValueLabel->setText(ACModeToString(homeCfg->AC.mode));
}

void MainWindow::on_volumeSlider_sliderMoved(int position)
{
    homeCfg->speakers.volume = position;
    ui->volumeSliderValueLabel->setText(QString::number(position));
}

void MainWindow::on_volumeSlider_valueChanged(int value)
{
    homeCfg->speakers.volume = value;
    ui->volumeSliderValueLabel->setText(QString::number(value));
}

void MainWindow::on_bassSlider_sliderMoved(int position)
{
    homeCfg->speakers.bass = position;
    ui->bassSliderValueLabel->setText(QString::number(position));
}

void MainWindow::on_bassSlider_valueChanged(int value)
{
    homeCfg->speakers.bass = value;
    ui->bassSliderValueLabel->setText(QString::number(value));
}

void MainWindow::on_pitchSlider_sliderMoved(int position)
{
    homeCfg->speakers.pitch = position;
    ui->pitchSliderValueLabel->setText(QString::number(position));
}

void MainWindow::on_pitchSlider_valueChanged(int value)
{
    homeCfg->speakers.pitch = value;
    ui->pitchSliderValueLabel->setText(QString::number(value));
}
