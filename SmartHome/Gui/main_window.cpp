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
#include "Pages/Devices/devices_page.h"
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

// TODO: Set to a lower value for easier testing
// constexpr int ONE_SEC_IN_TICKS = 20;
constexpr int ONE_SEC_IN_TICKS = 2;

MainWindow::MainWindow(smart_home::gui::GuiApp &app, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_session(&app) {
    ui->setupUi(this);
    connect(ui->devicesBtn, &QAbstractButton::clicked, this, &MainWindow::devicesBtnClicked);
    connect(ui->mediaBtn, &QAbstractButton::clicked, this, &MainWindow::mediaBtnClicked);
    connect(ui->analyticsBtn, &QAbstractButton::clicked, this, &MainWindow::analyticsBtnClicked);
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
    connect(m_saveAction, &QAction::triggered, m_session, &smart_home::gui::GuiApp::save);
    connect(m_reloadAction, &QAction::triggered, this, &MainWindow::reloadSettings);
    connect(m_session, &smart_home::gui::GuiApp::changed, this, &MainWindow::refreshSession);
    auto *devices = new DevicesPage(app, ui->pages);
    auto *placeholder = ui->pages->widget(0);
    ui->pages->removeWidget(placeholder);
    delete placeholder;
    ui->pages->insertWidget(0, devices);
    ui->pages->setCurrentIndex(0);
    refreshSession();

    initAnalyticsModel();
    ui->analyticsPages->setCurrentIndex(static_cast<int>(AnalyticsPageIndex::LIGHT_ANALYTICS));

    connect(&app, &smart_home::gui::GuiApp::tick, this, &MainWindow::onUpdate);
}

MainWindow::~MainWindow() {
    analyticsModel.reset();
    delete ui;
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
