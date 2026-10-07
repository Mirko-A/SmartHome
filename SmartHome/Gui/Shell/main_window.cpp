#include "main_window.h"

#include <QAction>
#include <QCloseEvent>
#include <QDateTime>
#include <QLabel>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

#include "Pages/Analytics/analytics_page.h"
#include "Pages/Devices/devices_page.h"
#include "Pages/Media/media_page.h"
#include "gui_app.h"
#include "ui_main_window.h"

using namespace smart_home;

MainWindow::MainWindow(gui::App &app, QWidget *parent)
    : QMainWindow(parent), m_app(app), m_ui(std::make_unique<Ui::MainWindow>()) {
    m_ui->setupUi(this);

    m_ui->pages->addWidget(new DevicesPage(app, m_ui->pages));
    m_ui->pages->addWidget(new MediaPage(m_ui->pages));
    m_ui->pages->addWidget(new AnalyticsPage(app, m_ui->pages));

    connect(m_ui->devicesBtn, &QPushButton::clicked, this, [this] { selectPage(0); });
    connect(m_ui->mediaBtn, &QPushButton::clicked, this, [this] { selectPage(1); });
    connect(m_ui->analyticsBtn, &QPushButton::clicked, this, [this] { selectPage(2); });

    selectPage(0);

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

    connect(m_saveAction, &QAction::triggered, &app, &gui::App::save);
    connect(m_reloadAction, &QAction::triggered, this, &MainWindow::reloadSettings);
    connect(&app, &gui::App::changed, this, &MainWindow::refreshSession);
    auto updateClock = [this] {
        m_ui->dateTimeLabel->setText(QDateTime::currentDateTimeUtc().toString());
    };
    connect(&app, &gui::App::tick, this, updateClock);

    updateClock();
    refreshSession();
}

MainWindow::~MainWindow() = default;

void MainWindow::selectPage(int index) {
    m_ui->pages->setCurrentIndex(index);
    QString fName = QString(QString(":/icons/three-dots-%1-purple.svg").arg(index));
    m_ui->buttonsCurrentButton->setIcon(QIcon(fName));
}
void MainWindow::refreshSession() {
    m_configStatus->setText(m_app.status());
    m_saveAction->setEnabled(m_app.loaded() && m_app.dirty() && !m_app.busy());
    m_reloadAction->setEnabled(!m_app.busy());
}
void MainWindow::reloadSettings() {
    if (m_app.dirty()) {
        const auto answer = QMessageBox::question(
            this, "Reload settings", "Discard pending edits if the configuration reload succeeds?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer != QMessageBox::Yes) {
            return;
        }
    }
    m_app.reload();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (m_app.busy()) {
        statusBar()->showMessage("Wait for the configuration operation to finish before closing.",
                                 4000);
        event->ignore();
        return;
    }
    if (m_app.dirty()) {
        const auto answer = QMessageBox::question(
            this, "Unsaved settings",
            "Discard pending edits and close? Use Save settings first to keep them.",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer == QMessageBox::Yes) {
            event->ignore();
            return;
        }
    }
    event->accept();
}
