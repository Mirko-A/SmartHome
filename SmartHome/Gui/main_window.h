#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QTimer>

#include "analytics_model.h"
#include "home_settings.h"
#include "media_player.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum class PageIndex {
    HOME,
    MEDIA,
    ANALYTICS,
};

enum class AnalyticsPageState {
    OFF,
    ON,
};

enum class AnalyticsPageIndex {
    LIGHT_ANALYTICS,
    AC_ANALYTICS,
    SENSORS_ANALYTICS,
};

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

  private slots:
    void onUpdate();

    /* Navigation bar button callbacks */
    void devicesBtnClicked();
    void mediaBtnClicked();
    void analyticsBtnClicked();

    /* Lights button callbacks */
    void livingRoomLightBtnToggled(bool checked);
    void bedroomLightBtnToggled(bool checked);
    void kitchenLightBtnToggled(bool checked);

    /* AC button callbacks */
    void acOnBtnToggled(bool checked);

    void acTemperatureUpClicked();
    void acTemperatureDownClicked();

    void acModeUpClicked();
    void acModeDownClicked();

    /* Speaker slider callbacks */
    void volumeSliderMoved(int position);
    void volumeSliderValueChanged(int value);

    void bassSliderMoved(int position);
    void bassSliderValueChanged(int value);

    void pitchSliderMoved(int position);
    void pitchSliderValueChanged(int value);

    void analyticsPageLightsBtnClicked();

    void analyticsPageACBtnClicked();

    void analyticsPageSensorsBtnClicked();

  private:
    void updateCurrentPage(PageIndex index);
    void updateDateTimeWidget();

    nlohmann::json loadHomeSettings();
    void saveHomeSettings();

    void updateLightsUI();
    void updateSensorsUI();
    void updateACUI();
    void updateSpeakersUI();
    void updateHomeWidgets();

    void loadHomeCfgWidgets();
    void reloadHomeWidgetsIfDirty();

    void loadMediaPlayerWidgets();
    void loadMediaPlayerControlWidgets();

    void initAnalyticsModel();

    void updateAnalyticsPageIcon(AnalyticsPageIndex pageIndex, AnalyticsPageState newState);
    void deselectCurrentAnalyticsPage();
    void selectNewAnalyticsPage(AnalyticsPageIndex newPageIndex);
    void swapSelectedAnalyticsPage(AnalyticsPageIndex newPageIndex);

    void updateUI();

  private:
    Ui::MainWindow *ui;

    HomeSettings *homeSettings;
    MediaPlayer *mediaPlayer;
    std::unique_ptr<AnalyticsModel> analyticsModel;

    QTimer *updateTimer;
};
#endif // MAINWINDOW_H
