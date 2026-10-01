#ifndef PLAYER_CONTROLS_H
#define PLAYER_CONTROLS_H

#include <QFrame>
#include <QMediaPlayer>
#include <memory>
namespace Ui {
class PlayerControls;
}

class QAbstractButton;
class QAbstractSlider;

class PlayerControls : public QFrame {
    Q_OBJECT

  public:
    PlayerControls(QWidget *parent = nullptr);

    ~PlayerControls() override;

    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;

  signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void changeVolume(int volume);
    void changeMuting(bool muting);

  public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);

  private slots:
    void playClicked();
    void muteClicked();

  private:
    void initializeUIElements();
    std::unique_ptr<Ui::PlayerControls> m_ui;
    QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
    bool m_playerMuted = false;
    bool m_uiInitialized = false;
};
#endif
