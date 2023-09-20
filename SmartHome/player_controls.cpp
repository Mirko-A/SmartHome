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
      m_playButton(0),
      m_stopButton(0),
      m_nextButton(0),
      m_previousButton(0),
      m_muteButton(0),
      m_volumeSlider(0),
      m_rateBox(0)
{
    m_playButton = new QToolButton(this);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playClicked()));

    m_stopButton = new QToolButton(this);
    m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    m_stopButton->setEnabled(false);

    connect(m_stopButton, SIGNAL(clicked()), this, SIGNAL(stop()));

    m_nextButton = new QToolButton(this);
    m_nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

    connect(m_nextButton, SIGNAL(clicked()), this, SIGNAL(next()));

    m_previousButton = new QToolButton(this);
    m_previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

    connect(m_previousButton, SIGNAL(clicked()), this, SIGNAL(previous()));

    m_muteButton = new QToolButton(this);
    m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    connect(m_muteButton, SIGNAL(clicked()), this, SLOT(muteClicked()));

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);

    connect(m_volumeSlider, SIGNAL(sliderMoved(int)), this, SIGNAL(changeVolume(int)));

    m_rateBox = new QComboBox(this);
    m_rateBox->addItem("0.5x", QVariant(0.5));
    m_rateBox->addItem("1.0x", QVariant(1.0));
    m_rateBox->addItem("2.0x", QVariant(2.0));
    m_rateBox->setCurrentIndex(1);

    connect(m_rateBox, SIGNAL(activated(int)), SLOT(updateRate()));

    QBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_stopButton);
    layout->addWidget(m_previousButton);
    layout->addWidget(m_playButton);
    layout->addWidget(m_nextButton);
    layout->addWidget(m_muteButton);
    layout->addWidget(m_volumeSlider);
    layout->addWidget(m_rateBox);
    setLayout(layout);
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
            m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        case QMediaPlayer::PlayingState:
            m_stopButton->setEnabled(true);
            m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        case QMediaPlayer::PausedState:
            m_stopButton->setEnabled(true);
            m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
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
    if (m_playerMuted != muted) {
        m_playerMuted = muted;

        m_muteButton->setIcon(style()->standardIcon(muted ?
                                                        QStyle::SP_MediaVolumeMuted :
                                                        QStyle::SP_MediaVolume));
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

qreal PlayerControls::playbackRate() const
{
    return m_rateBox->itemData(m_rateBox->currentIndex()).toDouble();
}

void PlayerControls::setPlaybackRate(float rate)
{
    for (int i = 0; i < m_rateBox->count(); i++)
    {
        if (qFuzzyCompare(rate, float(m_rateBox->itemData(i).toDouble())))
        {
            m_rateBox->setCurrentIndex(i);
            return;
        }
    }

    m_rateBox->addItem(QString("%1x").arg(rate), QVariant(rate));
    m_rateBox->setCurrentIndex(m_rateBox->count() - 1);
}

void PlayerControls::updateRate()
{
    emit changeRate(playbackRate());
}
