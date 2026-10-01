#include "media_player.h"

#include <QFileInfo>
#include <QMediaMetaData>
#include <QNetworkRequest>
#include <QVideoWidget>

MediaPlayer::MediaPlayer(QObject *parent)
    : QObject(parent), m_player(new QMediaPlayer(this)), m_playlist(new QMediaPlaylist(this)) {
    m_player->setPlaylist(m_playlist);
    m_player->setVolume(50);
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
        statusChanged(m_player->mediaStatus());
    });
    connect(m_playlist, &QMediaPlaylist::loadFailed, this, [this] {
        m_serviceError = false;
        m_errorInfo = m_playlist->errorString();
        if (m_errorInfo.isEmpty())
            m_errorInfo = tr("Unable to load playlist.");
        renderStatus();
    });

    connect(m_player, &QMediaPlayer::stateChanged, this, &MediaPlayer::stateChanged);
    connect(m_player, &QMediaPlayer::volumeChanged, this, &MediaPlayer::volumeChanged);
    connect(m_player, &QMediaPlayer::mutedChanged, this, &MediaPlayer::mutedChanged);
}
MediaPlayer::~MediaPlayer() {
    disconnect(m_player, nullptr, this, nullptr);
    disconnect(m_playlist, nullptr, this, nullptr);
    m_player->stop();
    m_player->setPlaylist(nullptr);
    delete m_player;
}
void MediaPlayer::publishState() {
    emit stateChanged(m_player->state());
    emit volumeChanged(m_player->volume());
    emit mutedChanged(m_player->isMuted());
    emit durationChanged(m_player->duration());
    emit positionChanged(m_player->position());
    emit playlistPositionChanged(m_playlist->currentIndex());
    metaDataChanged();
    statusChanged(m_player->mediaStatus());
    if (m_player->error() != QMediaPlayer::NoError)
        displayErrorMessage();
    refreshAvailability();
}
QStringList MediaPlayer::supportedMimeTypes() const {
    return m_player->supportedMimeTypes();
}
void MediaPlayer::setVideoOutput(QVideoWidget *video) {
    m_player->setVideoOutput(video);
}
void MediaPlayer::pause() {
    m_player->pause();
}
void MediaPlayer::next() {
    m_playlist->next();
}
void MediaPlayer::setVolume(int value) {
    m_player->setVolume(value);
}
void MediaPlayer::setMuted(bool muted) {
    m_player->setMuted(muted);
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
    m_player->setPosition(0);
    m_player->stop();
    if (m_player->mediaStatus() == QMediaPlayer::InvalidMedia ||
        m_player->error() != QMediaPlayer::NoError)
        displayErrorMessage();
}

void MediaPlayer::remove(int row) {
    if (!isPlayerAvailable() || row < 0 || row >= m_playlist->mediaCount())
        return;
    if (m_playlist->currentIndex() == row)
        stop();
    m_playlist->removeMedia(row);
}

static bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() &&
           !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void MediaPlayer::addToPlaylist(const QList<QUrl> &urls) {
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
}

void MediaPlayer::previous() {
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if (m_player->position() <= 5000)
        m_playlist->previous();
    else
        m_player->setPosition(0);
}

void MediaPlayer::jump(int row) {
    if (isPlayerAvailable() && row >= 0 && row < m_playlist->mediaCount()) {
        m_playlist->setCurrentIndex(row);
        play();
    }
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
