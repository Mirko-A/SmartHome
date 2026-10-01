#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QTimer>

#include "analytics_model.h"
#include "gui_session.h"

class QAction;
class QLabel;
class QCloseEvent;

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
    explicit MainWindow(std::string configPath, QWidget *parent = nullptr);
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

    void acModeUpClicked();
    void acModeDownClicked();

    /* Speaker slider callbacks */
    void volumeSliderValueChanged(int value);

    void bassSliderValueChanged(int value);

    void pitchSliderValueChanged(int value);

    void analyticsPageLightsBtnClicked();

    void analyticsPageACBtnClicked();

    void analyticsPageSensorsBtnClicked();

  private:
    void updateCurrentPage(PageIndex index);
    void updateDateTimeWidget();

    void refreshSession();
    void editControls();
    void reloadSettings();
    void closeEvent(QCloseEvent *event) override;

    void updateLightsUI();
    void updateSensorsUI();
    void updateACUI();
    void updateSpeakersUI();
    void updateHomeWidgets();

    void initAnalyticsModel();

    void updateAnalyticsPageIcon(AnalyticsPageIndex pageIndex, AnalyticsPageState newState);
    void deselectCurrentAnalyticsPage();
    void selectNewAnalyticsPage(AnalyticsPageIndex newPageIndex);
    void swapSelectedAnalyticsPage(AnalyticsPageIndex newPageIndex);

    void updateUI();

  private:
    Ui::MainWindow *ui;

    smart_home::gui::GuiSession *m_session;
    QAction *m_saveAction;
    QAction *m_reloadAction;
    QLabel *m_configStatus;
    std::unique_ptr<AnalyticsModel> analyticsModel;

    QTimer *updateTimer;
};
#endif // MAINWINDOW_H
