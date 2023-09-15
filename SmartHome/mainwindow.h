#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "home_config.h"

#include <QMainWindow>

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

private:
    void updateCurrentPage(PageIndex index);

private:
    Ui::MainWindow *ui;
    HomeConfig* homeCfg;
};
#endif // MAINWINDOW_H
