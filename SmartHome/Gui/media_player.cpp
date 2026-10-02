#include "media_player.h"

#include <QMediaMetaData>
#include <QMediaPlaylist>
#include <QMediaService>
#include <QSignalBlocker>
#include <QVideoProbe>

#include "playlist_model.h"

MediaPlayer::MediaPlayer(QWidget *parent)
    : QWidget(parent), m_videoWidget(nullptr), m_controls(nullptr), m_coverLabel(nullptr),
      m_seekSlider(nullptr) {
    m_player = new QMediaPlayer(this);
    // Owned by this player; PlaylistModel borrows it.
    m_playlist = new QMediaPlaylist(this);
    m_player->setPlaylist(m_playlist);

    m_videoWidget = new QVideoWidget(this);
    m_player->setVideoOutput(m_videoWidget);

    m_playlistModel = new PlaylistModel(this);
    m_playlistModel->setPlaylist(m_playlist);

    m_controls = new PlayerControls(this);
}

MediaPlayer::~MediaPlayer() {
    // Stop backend callbacks while the borrowed widgets are still alive.
    disconnect(m_player, nullptr, this, nullptr);
    disconnect(m_player, nullptr, m_controls, nullptr);
    disconnect(m_playlist, nullptr, this, nullptr);
    m_player->stop();
}

void MediaPlayer::initUi() {
    if (m_uiInitialized)
        return;

    Q_ASSERT(m_playlistView && m_labelDuration && m_seekSlider);
    Q_ASSERT(m_playlistModel->m_openButton && m_playlistModel->m_removeButton);
    m_controls->initializeUIElements();
    m_uiInitialized = true;

    connect(m_player, &QMediaPlayer::durationChanged, this, &MediaPlayer::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MediaPlayer::positionChanged);
    connect(m_player, static_cast<void (QMediaObject::*)()>(&QMediaObject::metaDataChanged), this,
            &MediaPlayer::metaDataChanged);
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, this,
            &MediaPlayer::playlistPositionChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MediaPlayer::statusChanged);
    connect(m_player, &QMediaPlayer::bufferStatusChanged, this, &MediaPlayer::bufferingProgress);
    connect(m_player,
            static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this,
            &MediaPlayer::displayErrorMessage);
    connect(m_player, static_cast<void (QMediaObject::*)(bool)>(&QMediaObject::availabilityChanged),
            this, &MediaPlayer::refreshAvailability);
    connect(m_player, &QMediaPlayer::currentMediaChanged, this, [this] {
        clearError();
        // Metadata from the previous track must not survive a media change.
        const QUrl url = m_player->currentMedia().request().url();
        setTrackInfo(url.fileName());
        if (m_coverLabel)
            m_coverLabel->clear();
        statusChanged(m_player->mediaStatus());
    });
    connect(m_playlist, &QMediaPlaylist::loadFailed, this, [this] {
        m_serviceError = false;
        m_errorInfo = m_playlist->errorString();
        if (m_errorInfo.isEmpty())
            m_errorInfo = tr("Unable to load playlist.");
        renderStatus();
    });

    m_playlistView->setModel(m_playlistModel);
    m_playlistView->setCurrentIndex(m_playlistModel->index(m_playlist->currentIndex(), 0));

    connect(m_playlistView, &QListView::activated, this, &MediaPlayer::jump);

    connect(m_seekSlider, &QSlider::valueChanged, this, &MediaPlayer::seek);

    connect(m_controls, &PlayerControls::play, this, &MediaPlayer::play);
    connect(m_controls, &PlayerControls::pause, m_player, &QMediaPlayer::pause);
    connect(m_controls, &PlayerControls::stop, this, &MediaPlayer::stop);
    connect(m_controls, &PlayerControls::next, m_playlist, &QMediaPlaylist::next);
    connect(m_controls, &PlayerControls::previous, this, &MediaPlayer::previousClicked);
    connect(m_controls, &PlayerControls::changeVolume, m_player, &QMediaPlayer::setVolume);
    connect(m_controls, &PlayerControls::changeMuting, m_player, &QMediaPlayer::setMuted);

    connect(m_player, &QMediaPlayer::stateChanged, m_controls, &PlayerControls::setState);
    connect(m_player, &QMediaPlayer::volumeChanged, m_controls, &PlayerControls::setVolume);
    connect(m_player, &QMediaPlayer::mutedChanged, m_controls, &PlayerControls::setMuted);

    connect(m_playlistModel->m_openButton, &QAbstractButton::clicked, this, &MediaPlayer::open);
    connect(m_playlistModel->m_removeButton, &QAbstractButton::clicked, this, &MediaPlayer::remove);

    connect(m_controls, &PlayerControls::stop, m_videoWidget,
            static_cast<void (QWidget::*)()>(&QWidget::update));

    // Render a complete initial snapshot after all borrowed widgets are bound.
    m_controls->setState(m_player->state());
    m_controls->setVolume(m_player->volume());
    m_controls->setMuted(m_player->isMuted());
    durationChanged(m_player->duration());
    positionChanged(m_player->position());

    metaDataChanged();
    statusChanged(m_player->mediaStatus());
    if (m_player->error() != QMediaPlayer::NoError)
        displayErrorMessage();
    refreshAvailability();
}

bool MediaPlayer::isPlayerAvailable() const {
    return m_player->isAvailable();
}

void MediaPlayer::refreshAvailability() {
    if (isPlayerAvailable() && m_serviceError)
        clearError();
    emit availabilityChanged(isPlayerAvailable());
    renderStatus();
}

void MediaPlayer::play() {
    if (!isPlayerAvailable())
        return;
    clearError();
    setStatusInfo(QString());
    m_player->play();
    if (m_player->mediaStatus() == QMediaPlayer::InvalidMedia ||
        m_player->error() != QMediaPlayer::NoError)
        displayErrorMessage();
}

void MediaPlayer::stop() {
    if (!isPlayerAvailable())
        return;
    clearError();
    setStatusInfo(QString());
    m_seekSlider->setValue(0);
    m_player->stop();
    if (m_player->mediaStatus() == QMediaPlayer::InvalidMedia ||
        m_player->error() != QMediaPlayer::NoError)
        displayErrorMessage();
}

void MediaPlayer::open() {
    if (!isPlayerAvailable())
        return;
    QFileDialog fileDialog(this);

    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));

    QStringList supportedMimeTypes = m_player->supportedMimeTypes();

    if (!supportedMimeTypes.isEmpty()) {
        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }

    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)
                                .value(0, QDir::homePath()));

    if (fileDialog.exec() == QDialog::Accepted)
        addToPlaylist(fileDialog.selectedUrls());
}

void MediaPlayer::remove() {
    if (!isPlayerAvailable())
        return;
    int indexToRemove = m_playlistView->currentIndex().row();
    if (indexToRemove < 0 || indexToRemove >= m_playlist->mediaCount())
        return;
    if (m_playlist->currentIndex() == indexToRemove) {
        m_seekSlider->setValue(0);
        m_player->stop();
    }

    m_playlist->removeMedia(indexToRemove);
}

static bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() &&
           !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void MediaPlayer::addToPlaylist(const QList<QUrl> urls) {
    if (!isPlayerAvailable() || urls.isEmpty())
        return;
    clearError();
    setStatusInfo(QString());
    for (const QUrl &url : urls) {
        if (isPlaylist(url))
            m_playlist->load(url);
        else
            m_playlist->addMedia(url);
    }
}

void MediaPlayer::durationChanged(qint64 duration) {
    this->m_duration = duration / 1000;
    // Prevent range clamping from emitting valueChanged and triggering a seek.
    const QSignalBlocker blocker(m_seekSlider);
    m_seekSlider->setRange(0, m_duration);
}

void MediaPlayer::positionChanged(qint64 progress) {
    if (!m_seekSlider->isSliderDown()) {
        // Display playback progress without emitting valueChanged and seeking back.
        const QSignalBlocker blocker(m_seekSlider);
        m_seekSlider->setValue(progress / 1000);
    }
    updateDurationInfo(progress / 1000);
}

void MediaPlayer::metaDataChanged() {
    QStringList trackParts;
    if (m_player->isMetaDataAvailable()) {
        for (const auto &key : {QMediaMetaData::AlbumArtist, QMediaMetaData::Title}) {
            const QString part = m_player->metaData(key).toString().trimmed();
            if (!part.isEmpty())
                trackParts.append(part);
        }
    }
    const QUrl url = m_player->currentMedia().request().url();
    setTrackInfo(trackParts.isEmpty() ? url.fileName() : trackParts.join(" - "));
    if (m_coverLabel) {
        const QUrl cover = m_player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();
        m_coverLabel->setPixmap(cover.isLocalFile() ? QPixmap(cover.toLocalFile()) : QPixmap());
    }
}

void MediaPlayer::previousClicked() {
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if (m_player->position() <= 5000)
        m_playlist->previous();
    else
        m_player->setPosition(0);
}

void MediaPlayer::jump(const QModelIndex &index) {
    if (isPlayerAvailable() && index.isValid()) {
        m_playlist->setCurrentIndex(index.row());
        play();
    }
}

void MediaPlayer::playlistPositionChanged(int currentItem) {
    m_playlistView->setCurrentIndex(m_playlistModel->index(currentItem, 0));
}

void MediaPlayer::seek(int seconds) {
    m_player->setPosition(qint64(seconds) * 1000);
}

void MediaPlayer::statusChanged(QMediaPlayer::MediaStatus status) {
    // handle status message
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::NoMedia:
        setTrackInfo(QString());
        setStatusInfo(tr("No media selected"));
        break;
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferedMedia:
        clearError();
        setStatusInfo(QString());
        break;
    case QMediaPlayer::BufferingMedia:
        bufferingProgress(m_player->bufferStatus());
        break;
    case QMediaPlayer::LoadingMedia:
        clearError();
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Media Stalled"));
        break;
    case QMediaPlayer::EndOfMedia:
        setStatusInfo(tr("Playback finished"));
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void MediaPlayer::bufferingProgress(int progress) {
    if (m_player->mediaStatus() == QMediaPlayer::BufferingMedia)
        setStatusInfo(tr("Buffering %1%").arg(progress));
}

void MediaPlayer::setTrackInfo(const QString &info) {
    m_trackInfo = info;
    renderStatus();
}

void MediaPlayer::setStatusInfo(const QString &info) {
    m_statusInfo = info;
    renderStatus();
}

void MediaPlayer::displayErrorMessage() {
    m_serviceError = m_player->error() == QMediaPlayer::ServiceMissingError;
    m_errorInfo = m_player->errorString();
    if (m_errorInfo.isEmpty())
        m_errorInfo = tr("Unable to play this media.");
    renderStatus();
}

void MediaPlayer::clearError() {
    m_errorInfo.clear();
    m_serviceError = false;
}

void MediaPlayer::renderStatus() {
    const bool available = isPlayerAvailable();
    QString status = m_errorInfo.isEmpty() ? m_statusInfo : m_errorInfo;
    if (!available)
        status =
            tr("Media service unavailable. Check that the media service plugins are installed.");

    QStringList parts;
    if (!m_trackInfo.isEmpty())
        parts.append(m_trackInfo);
    if (!status.isEmpty())
        parts.append(status);
    emit statusTextChanged(parts.isEmpty() ? tr("No media selected") : parts.join(" | "));

    const auto mediaStatus = m_player->mediaStatus();
    emit busyChanged(available && m_errorInfo.isEmpty() &&
                     (mediaStatus == QMediaPlayer::LoadingMedia ||
                      mediaStatus == QMediaPlayer::BufferingMedia ||
                      mediaStatus == QMediaPlayer::StalledMedia));
}

void MediaPlayer::updateDurationInfo(qint64 currentInfo) {
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60, currentInfo % 60,
                          (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60, m_duration % 60,
                        (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_labelDuration->setText(tStr);
}
