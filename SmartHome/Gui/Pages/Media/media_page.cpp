#include "media_page.h"

#include <QFileDialog>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QTime>
#include <QVideoWidget>

#include "media_player.h"
#include "playlist_model.h"
#include "ui_media_page.h"

MediaPage::MediaPage(QWidget *parent)
    : QWidget(parent), m_ui(std::make_unique<Ui::MediaPage>()), m_player(new MediaPlayer(this)),
      m_playlistModel(new PlaylistModel(this)) {
    m_ui->setupUi(this);

    // Setup video.
    auto *video = new QVideoWidget(this);
    m_player->setVideoOutput(video);
    m_ui->mediaPlayerVideoWidgetContainer->addWidget(video);

    // Setup playlist.
    m_playlistModel->setPlaylist(m_player->playlist());
    m_ui->mediaPlaylistListView->setModel(m_playlistModel);

    // Qt - media controls.
    auto *controls = m_ui->mediaPlayerControlsBottom;
    connect(controls, &PlayerControls::play, m_player, &MediaPlayer::play);
    connect(controls, &PlayerControls::pause, m_player, &MediaPlayer::pause);
    connect(controls, &PlayerControls::stop, m_player, &MediaPlayer::stop);
    connect(controls, &PlayerControls::next, m_player, &MediaPlayer::next);
    connect(controls, &PlayerControls::previous, m_player, &MediaPlayer::previous);
    connect(controls, &PlayerControls::changeVolume, m_player, &MediaPlayer::setVolume);
    connect(controls, &PlayerControls::changeMuting, m_player, &MediaPlayer::setMuted);

    connect(m_player, &MediaPlayer::stateChanged, controls, &PlayerControls::setState);
    connect(m_player, &MediaPlayer::volumeChanged, controls, &PlayerControls::setVolume);
    connect(m_player, &MediaPlayer::mutedChanged, controls, &PlayerControls::setMuted);

    // Qt - playlist controls.
    connect(m_ui->mediaPlaylistOpenBtn, &QPushButton::clicked, this, &MediaPage::open);
    connect(m_ui->mediaPlaylistRemoveBtn, &QPushButton::clicked, this,
            [this] { m_player->remove(m_ui->mediaPlaylistListView->currentIndex().row()); });
    connect(m_ui->mediaPlaylistListView, &QListView::activated, this,
            [this](const QModelIndex &index) {
                if (index.isValid()) {
                    m_player->jump(index.row());
                }
            });
    connect(m_player, &MediaPlayer::playlistPositionChanged, this, [this](int row) {
        m_ui->mediaPlaylistListView->setCurrentIndex(m_playlistModel->index(row, 0));
    });

    // Qt - seek slider controls.
    connect(m_ui->mediaPlayerSeekSlider, &QSlider::valueChanged, m_player, &MediaPlayer::seek);
    connect(m_player, &MediaPlayer::durationChanged, this, [this](qint64 duration) {
        m_duration = duration / 1000;
        const QSignalBlocker blocker(m_ui->mediaPlayerSeekSlider);
        m_ui->mediaPlayerSeekSlider->setRange(0, m_duration);
        updateDuration();
    });
    connect(m_player, &MediaPlayer::positionChanged, this, [this](qint64 position) {
        m_position = position / 1000;
        if (!m_ui->mediaPlayerSeekSlider->isSliderDown()) {
            const QSignalBlocker blocker(m_ui->mediaPlayerSeekSlider);
            m_ui->mediaPlayerSeekSlider->setValue(m_position);
        }
        updateDuration();
    });

    // Qt - extra player signals.
    connect(m_player, &MediaPlayer::availabilityChanged, m_ui->mediaPlayer, &QWidget::setEnabled);
    connect(m_player, &MediaPlayer::statusTextChanged, m_ui->mediaStatusLabel, &QLabel::setText);
    connect(m_player, &MediaPlayer::busyChanged, this, [this](bool busy) {
        if (busy)
            m_ui->mediaPlayer->setCursor(Qt::BusyCursor);
        else
            m_ui->mediaPlayer->unsetCursor();
    });

    m_player->publishState();
}

MediaPage::~MediaPage() {
    // Stop backend callbacks before page widgets and their UI wrapper are destroyed.
    disconnect(m_player, nullptr, this, nullptr);
    delete m_player;
}

void MediaPage::open() {
    if (!m_player->isPlayerAvailable()) {
        return;
    }

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setWindowTitle(tr("Open Files"));

    auto types = m_player->supportedMimeTypes();
    if (!types.isEmpty()) {
        types.append("audio/x-m3u");
        dialog.setMimeTypeFilters(types);
    }

    dialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)
                            .value(0, QDir::homePath()));

    if (dialog.exec() == QDialog::Accepted) {
        m_player->addToPlaylist(dialog.selectedUrls());
    }
}

void MediaPage::updateDuration() {
    auto format = [this](qint64 seconds) {
        const auto time = QTime(0, 0).addSecs(static_cast<int>(seconds));
        return time.toString(m_duration >= 3600 ? "hh:mm:ss" : "mm:ss");
    };

    QString duration = QString(format(m_position) + " / " + format(m_duration));
    m_ui->mediaPlayerDurationLabel->setText(m_position || m_duration ? duration : QString());
}
