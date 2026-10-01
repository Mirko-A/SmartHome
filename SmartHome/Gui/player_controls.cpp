#include "player_controls.h"

#include <QBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QStyle>
#include <QToolButton>

PlayerControls::PlayerControls(QWidget *parent)
    : QWidget(parent), m_playerState(QMediaPlayer::StoppedState), m_playerMuted(false),
      m_playButton(nullptr), m_stopButton(nullptr), m_nextButton(nullptr),
      m_previousButton(nullptr), m_muteButton(nullptr), m_volumeSlider(nullptr) {}

void PlayerControls::initializeUIElements() {
    if (m_uiInitialized)
        return;
    Q_ASSERT(m_playButton && m_stopButton && m_nextButton && m_previousButton && m_muteButton &&
             m_volumeSlider);
    m_uiInitialized = true;
    setState(m_playerState);
    setMuted(m_playerMuted);

    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    connect(m_stopButton, SIGNAL(clicked()), this, SIGNAL(stop()));
    connect(m_nextButton, SIGNAL(clicked()), this, SIGNAL(next()));
    connect(m_previousButton, SIGNAL(clicked()), this, SIGNAL(previous()));
    connect(m_muteButton, SIGNAL(clicked()), this, SLOT(muteClicked()));
    connect(m_volumeSlider, SIGNAL(sliderMoved(int)), this, SIGNAL(changeVolume(int)));
}

QMediaPlayer::State PlayerControls::state() const {
    return m_playerState;
}

void PlayerControls::setState(QMediaPlayer::State state) {
    m_playerState = state;
    if (!m_uiInitialized)
        return;

    switch (state) {
    case QMediaPlayer::StoppedState:
        m_stopButton->setEnabled(false);
        m_playButton->setIcon(QIcon(":/icons/play.svg"));
        break;
    case QMediaPlayer::PlayingState:
        m_stopButton->setEnabled(true);
        m_playButton->setIcon(QIcon(":/icons/pause.svg"));
        break;
    case QMediaPlayer::PausedState:
        m_stopButton->setEnabled(true);
        m_playButton->setIcon(QIcon(":/icons/play.svg"));
        break;
    }
}

int PlayerControls::volume() const {
    return m_volumeSlider ? m_volumeSlider->value() : 0;
}

void PlayerControls::setVolume(int volume) {
    if (m_volumeSlider)
        m_volumeSlider->setValue(volume);
}

bool PlayerControls::isMuted() const {
    return m_playerMuted;
}

void PlayerControls::setMuted(bool muted) {
    m_playerMuted = muted;
    if (m_uiInitialized)
        m_muteButton->setIcon(QIcon(muted ? ":/icons/volume-x.svg" : ":/icons/volume-2.svg"));
}

void PlayerControls::playClicked() {
    switch (m_playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    }
}

void PlayerControls::muteClicked() {
    emit changeMuting(!m_playerMuted);
}
