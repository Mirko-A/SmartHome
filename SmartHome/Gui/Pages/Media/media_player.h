#pragma once

#include <QList>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QObject>
#include <QUrl>

class QVideoWidget;

// Playback backend only. The page owns all widgets and the playlist model.
class MediaPlayer : public QObject {
    Q_OBJECT

  public:
    explicit MediaPlayer(QObject *parent = nullptr);
    ~MediaPlayer() override;

    QMediaPlaylist *playlist() const {
        return m_playlist;
    }

    bool isPlayerAvailable() const;
    QStringList supportedMimeTypes() const;
    void setVideoOutput(QVideoWidget *video);
    void publishState();
    void addToPlaylist(const QList<QUrl> &urls);

  public slots:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void seek(int seconds);
    void jump(int row);
    void remove(int row);
    void setVolume(int value);
    void setMuted(bool muted);

  signals:
    void availabilityChanged(bool available);
    void statusTextChanged(const QString &text);
    void busyChanged(bool busy);
    void stateChanged(QMediaPlayer::State state);
    void volumeChanged(int volume);
    void mutedChanged(bool muted);
    void durationChanged(qint64 durationMs);
    void positionChanged(qint64 positionMs);
    void playlistPositionChanged(int row);

  private:
    void refreshAvailability();
    void metaDataChanged();
    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);
    void displayErrorMessage();
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void renderStatus();
    void clearError();

  private:
    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;
    QString m_trackInfo;
    QString m_statusInfo;
    bool m_serviceError = false;
    QString m_errorInfo;
};
