#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QTimer>

#include "analytics_model.h"
#include "gui_app.h"

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
    explicit MainWindow(smart_home::gui::GuiApp &app, QWidget *parent = nullptr);
    ~MainWindow() override;

  private slots:
    void onUpdate();

    /* Navigation bar button callbacks */
    void devicesBtnClicked();
    void mediaBtnClicked();
    void analyticsBtnClicked();

    void analyticsPageLightsBtnClicked();

    void analyticsPageACBtnClicked();

    void analyticsPageSensorsBtnClicked();

  private:
    void updateCurrentPage(PageIndex index);
    void updateDateTimeWidget();

    void refreshSession();
    void reloadSettings();
    void closeEvent(QCloseEvent *event) override;

    void initAnalyticsModel();

    void updateAnalyticsPageIcon(AnalyticsPageIndex pageIndex, AnalyticsPageState newState);
    void deselectCurrentAnalyticsPage();
    void selectNewAnalyticsPage(AnalyticsPageIndex newPageIndex);
    void swapSelectedAnalyticsPage(AnalyticsPageIndex newPageIndex);

    void updateUI();

  private:
    Ui::MainWindow *ui;

    smart_home::gui::GuiApp *m_session;
    QAction *m_saveAction;
    QAction *m_reloadAction;
    QLabel *m_configStatus;
    std::unique_ptr<AnalyticsModel> analyticsModel;
};
#endif // MAINWINDOW_H
