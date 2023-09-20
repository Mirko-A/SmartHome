#ifndef PLAYER_CONTROLS_H
#define PLAYER_CONTROLS_H

#include <QMediaPlayer>
#include <QWidget>

class QAbstractButton;
class QAbstractSlider;
class QComboBox;

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget* parent = nullptr);

    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;
    qreal playbackRate() const;

public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);
    void setPlaybackRate(float rate);

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void changeVolume(int volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);

private slots:
    void playClicked();
    void muteClicked();
    void updateRate();

private:
    QMediaPlayer::State m_playerState;
    bool m_playerMuted;

    QAbstractButton* m_playButton;
    QAbstractButton* m_stopButton;
    QAbstractButton* m_nextButton;
    QAbstractButton* m_previousButton;
    QAbstractButton* m_muteButton;
    QAbstractSlider* m_volumeSlider;
    QComboBox* m_rateBox;
};

#endif // PLAYER_CONTROLS_H
