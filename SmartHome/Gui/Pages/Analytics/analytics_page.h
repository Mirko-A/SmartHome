#pragma once
#include <QWidget>
#include <array>
#include <memory>

#include "Charts/histogram.h"
#include "Charts/line_graph.h"
namespace Ui {
class AnalyticsPage;
}
namespace smart_home::gui {
class GuiApp;
}
class AnalyticsPage : public QWidget {
    Q_OBJECT
  public:
    explicit AnalyticsPage(smart_home::gui::GuiApp &app, QWidget *parent = nullptr);
    ~AnalyticsPage() override;

  private:
    void selectPage(int index);
    void render();
    smart_home::gui::GuiApp &m_app;
    std::unique_ptr<Ui::AnalyticsPage> m_ui;
    std::array<std::unique_ptr<Histogram>, 4> m_histograms;
    std::array<std::unique_ptr<LineGraph>, 3> m_graphs;
};
