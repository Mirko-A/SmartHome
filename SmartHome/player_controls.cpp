#include "player_controls.h"

#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>

PlayerControls::PlayerControls(QWidget* parent)
    : QWidget(parent),
      m_playerState(QMediaPlayer::StoppedState),
      m_playerMuted(false),
      m_playButton(nullptr),
      m_stopButton(nullptr),
      m_nextButton(nullptr),
      m_previousButton(nullptr),
      m_muteButton(nullptr),
      m_volumeSlider(nullptr),
      m_openButton(nullptr),
      m_removeButton(nullptr)
{

}

void PlayerControls::initializeUIElements()
{
    m_stopButton->setEnabled(false);

    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    connect(m_stopButton, SIGNAL(clicked()), this, SIGNAL(stop()));
    connect(m_nextButton, SIGNAL(clicked()), this, SIGNAL(next()));
    connect(m_previousButton, SIGNAL(clicked()), this, SIGNAL(previous()));
    connect(m_muteButton, SIGNAL(clicked()), this, SLOT(muteClicked()));
    connect(m_volumeSlider, SIGNAL(sliderMoved(int)), this, SIGNAL(changeVolume(int)));
}

QMediaPlayer::State PlayerControls::state() const
{
    return m_playerState;
}

void PlayerControls::setState(QMediaPlayer::State state)
{
    if (state != m_playerState) {
        m_playerState = state;

        switch (state) {
        case QMediaPlayer::StoppedState:
            m_stopButton->setEnabled(false);
            m_playButton->setIcon(QIcon("../resource/icons/play.svg"));
            break;
        case QMediaPlayer::PlayingState:
            m_stopButton->setEnabled(true);
            m_playButton->setIcon(QIcon("../resource/icons/pause.svg"));
            break;
        case QMediaPlayer::PausedState:
            m_stopButton->setEnabled(true);
            m_playButton->setIcon(QIcon("../resource/icons/play.svg"));
            break;
        }
    }
}

int PlayerControls::volume() const
{
    return m_volumeSlider ? m_volumeSlider->value() : 0;
}

void PlayerControls::setVolume(int volume)
{
    if (m_volumeSlider)
        m_volumeSlider->setValue(volume);
}

bool PlayerControls::isMuted() const
{
    return m_playerMuted;
}

void PlayerControls::setMuted(bool muted)
{
    if (m_playerMuted != muted)
    {
        m_playerMuted = muted;

        m_muteButton->setIcon(QIcon(muted ? "../resource/icons/volume-x.svg" : "../resource/icons/volume-2.svg"));
    }
}

void PlayerControls::playClicked()
{
    switch (m_playerState)
    {
        case QMediaPlayer::StoppedState:
        case QMediaPlayer::PausedState:
            emit play();
            break;
        case QMediaPlayer::PlayingState:
            emit pause();
            break;
    }
}

void PlayerControls::muteClicked()
{
    emit changeMuting(!m_playerMuted);
}
