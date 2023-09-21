#ifndef PLAYER_CONTROLS_H
#define PLAYER_CONTROLS_H

#include <QMediaPlayer>
#include <QWidget>

class QAbstractButton;
class QAbstractSlider;

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget* parent = nullptr);

    /* Must be called after pointers to UI elements have been assigned */
    void initializeUIElements();

    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;

public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void changeVolume(int volume);
    void changeMuting(bool muting);

private slots:
    void playClicked();
    void muteClicked();

public:
    QMediaPlayer::State m_playerState;
    bool m_playerMuted;

    /* Part of player */
    QAbstractButton* m_playButton;
    QAbstractButton* m_stopButton;
    QAbstractButton* m_nextButton;
    QAbstractButton* m_previousButton;
    QAbstractButton* m_muteButton;
    QAbstractSlider* m_volumeSlider;

    /* Part of playlist */
    QAbstractButton* m_openButton;
    QAbstractButton* m_removeButton;
};

#endif // PLAYER_CONTROLS_H
