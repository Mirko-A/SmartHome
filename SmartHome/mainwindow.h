#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "home_config.h"

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class PageIndex
{
    HOME,
    MUSIC,
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
    void on_musicBtn_clicked();
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

private:
    void updateCurrentPage(PageIndex index);
    void updateDateTimeWidget();

    void loadCfgFromJSON();
    void saveCfgAsJSON();

    void updateLightsUI();
    void updateSensorsUI();
    void updateACUI();
    void updateSpeakersUI();
    void updateUI();

private:
    Ui::MainWindow *ui;
    HomeConfig* homeCfg;
    QTimer* updateTimer;
};
#endif // MAINWINDOW_H
