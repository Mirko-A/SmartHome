#include "media_player.h"

#include "playlist_model.h"

#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QMediaMetaData>

MediaPlayer::MediaPlayer(QWidget *parent)
    : QWidget(parent),
      m_videoWidget(nullptr),
      m_controls(nullptr),
      m_coverLabel(nullptr),
      m_seekSlider(nullptr)
{

    m_player = new QMediaPlayer(this);
    // owned by PlaylistModel
    m_playlist = new QMediaPlaylist(this);
    m_player->setPlaylist(m_playlist);

    m_videoWidget = new QVideoWidget(this);
    m_player->setVideoOutput(m_videoWidget);

    connect(m_player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(m_player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(m_player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
    connect(m_playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));
    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this,     SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));
    connect(m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));

    m_playlistModel = new PlaylistModel(this);
    m_playlistModel->setPlaylist(m_playlist);

    m_controls = new PlayerControls(this);
    m_controls->setState(m_player->state());
    m_controls->setVolume(m_player->volume());
    m_controls->setMuted(m_controls->isMuted());
    m_controls->setEnabled(true);
}

MediaPlayer::~MediaPlayer()
{

}

void MediaPlayer::initializeUIElements()
{
    m_playlistView->setModel(m_playlistModel);
    m_playlistView->setCurrentIndex(m_playlistModel->index(m_playlist->currentIndex(), 0));

    connect(m_playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

    m_seekSlider->setRange(0, m_player->duration() / 1000);

    connect(m_seekSlider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));

    connect(m_controls, SIGNAL(play()), m_player, SLOT(play()));
    connect(m_controls, SIGNAL(pause()), m_player, SLOT(pause()));
    connect(m_controls, SIGNAL(stop()), m_player, SLOT(stop()));
    connect(m_controls, SIGNAL(next()), m_playlist, SLOT(next()));
    connect(m_controls, SIGNAL(previous()), this, SLOT(previousClicked()));
    connect(m_controls, SIGNAL(changeVolume(int)), m_player, SLOT(setVolume(int)));
    connect(m_controls, SIGNAL(changeMuting(bool)), m_player, SLOT(setMuted(bool)));

    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)),
            m_controls, SLOT(setState(QMediaPlayer::State)));
    connect(m_player, SIGNAL(volumeChanged(int)), m_controls, SLOT(setVolume(int)));
    connect(m_player, SIGNAL(mutedChanged(bool)), m_controls, SLOT(setMuted(bool)));

    connect(m_playlistModel->m_openButton, SIGNAL(clicked()), this, SLOT(open()));
    connect(m_playlistModel->m_removeButton, SIGNAL(clicked()), this, SLOT(remove()));

    connect(m_controls, SIGNAL(stop()), m_videoWidget, SLOT(update()));

    if (!isPlayerAvailable()) {
        QMessageBox::warning(this, tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"\
                                "Please check the media service plugins are installed."));

        m_controls->setEnabled(false);
        m_playlistView->setEnabled(false);
    }

    metaDataChanged();
}

bool MediaPlayer::isPlayerAvailable() const
{
    return m_player->isAvailable();
}

void MediaPlayer::open()
{
    QFileDialog fileDialog(this);

    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));

    QStringList supportedMimeTypes = m_player->supportedMimeTypes();

    if (!supportedMimeTypes.isEmpty())
    {
        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }

    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));

    if (fileDialog.exec() == QDialog::Accepted)
        addToPlaylist(fileDialog.selectedUrls());
}

void MediaPlayer::remove()
{

}

static bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void MediaPlayer::addToPlaylist(const QList<QUrl> urls)
{
    foreach (const QUrl &url, urls) {
        if (isPlaylist(url))
            m_playlist->load(url);
        else
            m_playlist->addMedia(url);
    }
}

void MediaPlayer::durationChanged(qint64 duration)
{
    this->m_duration = duration/1000;
    m_seekSlider->setMaximum(duration / 1000);
}

void MediaPlayer::positionChanged(qint64 progress)
{
    if (!m_seekSlider->isSliderDown()) {
        m_seekSlider->setValue(progress / 1000);
    }
    updateDurationInfo(progress / 1000);
}

void MediaPlayer::metaDataChanged()
{
    if (m_player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(m_player->metaData(QMediaMetaData::AlbumArtist).toString())
                .arg(m_player->metaData(QMediaMetaData::Title).toString()));

        if (m_coverLabel) {
            QUrl url = m_player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

            m_coverLabel->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }
    }
}

void MediaPlayer::previousClicked()
{
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if(m_player->position() <= 5000)
        m_playlist->previous();
    else
        m_player->setPosition(0);
}

void MediaPlayer::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        m_playlist->setCurrentIndex(index.row());
        m_player->play();
    }
}

void MediaPlayer::playlistPositionChanged(int currentItem)
{
    m_playlistView->setCurrentIndex(m_playlistModel->index(currentItem, 0));
}

void MediaPlayer::seek(int seconds)
{
    m_player->setPosition(seconds * 1000);
}

void MediaPlayer::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Media Stalled"));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void MediaPlayer::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia ||
        status == QMediaPlayer::BufferingMedia ||
        status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void MediaPlayer::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%").arg(progress));
}

void MediaPlayer::setTrackInfo(const QString &info)
{
    m_trackInfo = info;
    if (!m_statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
    else
        setWindowTitle(m_trackInfo);
}

void MediaPlayer::setStatusInfo(const QString &info)
{
    m_statusInfo = info;
    if (!m_statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
    else
        setWindowTitle(m_trackInfo);
}

void MediaPlayer::displayErrorMessage()
{
    setStatusInfo(m_player->errorString());
}

void MediaPlayer::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
        QTime totalTime((m_duration/3600)%60, (m_duration/60)%60, m_duration%60, (m_duration*1000)%1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_labelDuration->setText(tStr);
}
