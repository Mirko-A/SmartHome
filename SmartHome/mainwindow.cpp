#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>

const QVector<QString> PAGE_ICON_PATHS =
{
  "resource/icons/three-dots-0.svg",
  "resource/icons/three-dots-1.svg",
  "resource/icons/three-dots-2.svg",
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pages->setCurrentIndex(static_cast<int>(PageIndex::HOME)); // Set the initial tab to HOME tab

    homeCfg = new HomeConfig;

    QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
    ui->dateTimeLabel->setText(currentDateTime.toString());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete homeCfg;
}

void MainWindow::updateCurrentPage(PageIndex index)
{
    ui->pages->setCurrentIndex(static_cast<int>(index)); // reveal the home page
    ui->buttonsCurrentButton->setIcon(QIcon(PAGE_ICON_PATHS.at(static_cast<int>(index)))); // update current page icon
}

void MainWindow::on_devicesBtn_clicked()
{
    updateCurrentPage(PageIndex::HOME);
    // TODO: MOVE THIS TO SEPARATE FUNCTION
    QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
    ui->dateTimeLabel->setText(currentDateTime.toString());
}

void MainWindow::on_musicBtn_clicked()
{
    updateCurrentPage(PageIndex::MUSIC);
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
        ui->livingRoomLightBtn->setIcon(QIcon("resource/icons/toggle-on-colored.svg"));
    }
    else
    {
        ui->livingRoomLightBtn->setIcon(QIcon("resource/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_bedroomLightBtn_toggled(bool checked)
{
    // TODO: handle actual light
    homeCfg->lights.bedroomLightOn = checked;

    if (checked)
    {
        ui->bedroomLightBtn->setIcon(QIcon("resource/icons/toggle-on-colored.svg"));
    }
    else
    {
        ui->bedroomLightBtn->setIcon(QIcon("resource/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_kitchenLightBtn_toggled(bool checked)
{
    // TODO: handle actual light
    homeCfg->lights.kitchenLightOn = checked;

    if (checked)
    {
        ui->kitchenLightBtn->setIcon(QIcon("resource/icons/toggle-on-colored.svg"));
    }
    else
    {
        ui->kitchenLightBtn->setIcon(QIcon("resource/icons/toggle-off-colored.svg"));
    }
}

void MainWindow::on_ACOnBtn_toggled(bool checked)
{
    // TODO: handle actual light
    homeCfg->AC.on = checked;

    if (checked)
    {
        ui->ACOnBtn->setIcon(QIcon("resource/icons/toggle-on-colored.svg"));
    }
    else
    {
        ui->ACOnBtn->setIcon(QIcon("resource/icons/toggle-off-colored.svg"));
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

