#include "player_controls.h"

#include <QBoxLayout>
#include <QComboBox>
#include <QSignalBlocker>
#include <QSlider>
#include <QStyle>
#include <QToolButton>

#include "ui_player_controls.h"

PlayerControls::PlayerControls(QWidget *parent)
    : QFrame(parent), m_ui(std::make_unique<Ui::PlayerControls>()) {
    m_ui->setupUi(this);

    Q_ASSERT(m_ui->mediaPlayerPlayBtn && m_ui->mediaPlayerStopBtn && m_ui->mediaPlayerNextBtn &&
             m_ui->mediaPlayerPrevBtn && m_ui->mediaPlayerMuteBtn && m_ui->mediaPlayerVolumeSlider);
    m_uiInitialized = true;

    setState(m_playerState);
    setMuted(m_playerMuted);

    m_ui->mediaPlayerVolumeSlider->setRange(0, 100);

    connect(m_ui->mediaPlayerPlayBtn, &QAbstractButton::clicked, this,
            &PlayerControls::playClicked);
    connect(m_ui->mediaPlayerStopBtn, &QAbstractButton::clicked, this, &PlayerControls::stop);
    connect(m_ui->mediaPlayerNextBtn, &QAbstractButton::clicked, this, &PlayerControls::next);
    connect(m_ui->mediaPlayerPrevBtn, &QAbstractButton::clicked, this, &PlayerControls::previous);
    connect(m_ui->mediaPlayerMuteBtn, &QAbstractButton::clicked, this,
            &PlayerControls::muteClicked);
    connect(m_ui->mediaPlayerVolumeSlider, &QAbstractSlider::valueChanged, this,
            &PlayerControls::changeVolume);
}

PlayerControls::~PlayerControls() = default;

QMediaPlayer::State PlayerControls::state() const {
    return m_playerState;
}

void PlayerControls::setState(QMediaPlayer::State state) {
    m_playerState = state;
    if (!m_uiInitialized) {
        return;
    }

    switch (state) {
    case QMediaPlayer::StoppedState:
        m_ui->mediaPlayerStopBtn->setEnabled(false);
        m_ui->mediaPlayerPlayBtn->setIcon(QIcon(":/icons/play.svg"));
        break;
    case QMediaPlayer::PlayingState:
        m_ui->mediaPlayerStopBtn->setEnabled(true);
        m_ui->mediaPlayerPlayBtn->setIcon(QIcon(":/icons/pause.svg"));
        break;
    case QMediaPlayer::PausedState:
        m_ui->mediaPlayerStopBtn->setEnabled(true);
        m_ui->mediaPlayerPlayBtn->setIcon(QIcon(":/icons/play.svg"));
        break;
    }
}

int PlayerControls::volume() const {
    return m_ui->mediaPlayerVolumeSlider ? m_ui->mediaPlayerVolumeSlider->value() : 0;
}

void PlayerControls::setVolume(int volume) {
    if (m_ui->mediaPlayerVolumeSlider) {
        // Display player volume without emitting valueChanged and
        // sending it back to the player.
        const QSignalBlocker blocker(m_ui->mediaPlayerVolumeSlider);
        m_ui->mediaPlayerVolumeSlider->setValue(volume);
    }
}

bool PlayerControls::isMuted() const {
    return m_playerMuted;
}

void PlayerControls::setMuted(bool muted) {
    m_playerMuted = muted;
    if (m_uiInitialized) {
        m_ui->mediaPlayerMuteBtn->setIcon(
            QIcon(muted ? ":/icons/volume-x.svg" : ":/icons/volume-2.svg"));
    }
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
