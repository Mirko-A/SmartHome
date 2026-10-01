#pragma once
#include <QWidget>
#include <memory>
namespace Ui {
class MediaPage;
}
class MediaPlayer;
class PlaylistModel;
class MediaPage : public QWidget {
    Q_OBJECT
  public:
    explicit MediaPage(QWidget *parent = nullptr);
    ~MediaPage() override;

  private:
    void open();
    void updateDuration();
    std::unique_ptr<Ui::MediaPage> m_ui;
    MediaPlayer *m_player;
    PlaylistModel *m_playlistModel;
    qint64 m_duration = 0;
    qint64 m_position = 0;
};
