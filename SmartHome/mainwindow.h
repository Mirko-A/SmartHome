#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "home_config.h"
#include "media_player.h"
#include "analytics_model.h"

#include <QMainWindow>
#include <QMediaPlayer>

#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class PageIndex
{
    HOME,
    MEDIA,
    ANALYTICS,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onUpdate();

    /* Navigation bar button callbacks */
    void on_devicesBtn_clicked();
    void on_mediaBtn_clicked();
    void on_analyticsBtn_clicked();

    /* Lights button callbacks */
    void on_livingRoomLightBtn_toggled(bool checked);
    void on_bedroomLightBtn_toggled(bool checked);
    void on_kitchenLightBtn_toggled(bool checked);

    /* AC button callbacks */
    void on_ACOnBtn_toggled(bool checked);

    void on_ACTemperatureUp_clicked();
    void on_ACTemperatureDown_clicked();

    void on_ACModeUp_clicked();
    void on_ACModeDown_clicked();

    /* Speaker slider callbacks */
    void on_volumeSlider_sliderMoved(int position);
    void on_volumeSlider_valueChanged(int value);

    void on_bassSlider_sliderMoved(int position);
    void on_bassSlider_valueChanged(int value);

    void on_pitchSlider_sliderMoved(int position);
    void on_pitchSlider_valueChanged(int value);

private:
    void updateCurrentPage(PageIndex index);
    void updateDateTimeWidget();

    nlohmann::json loadHomeCfgAsJson();
    void saveHomeCfgAsJSON();

    void updateLightsUI();
    void updateSensorsUI();
    void updateACUI();
    void updateSpeakersUI();
    void updateHomeCfgWidgets();

    void loadHomeCfgWidgets();
    void reloadHomeCfgWidgetsIfDirty();

    void loadMediaPlayerWidgets();
    void loadMediaPlayerControlWidgets();

    void initAnalyticsModel();
    void setAnalyticsModelCharts();
    void loadAnalyticsModelWidgets();

    void updateUI();

private:
    Ui::MainWindow *ui;

    HomeConfig* homeCfg;
    MediaPlayer* mediaPlayer;
    AnalyticsModel* analyticsModel;

    QTimer* updateTimer;
};
#endif // MAINWINDOW_H
