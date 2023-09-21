#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include "playlist_model.h"
#include "player_controls.h"

#include <QWidget>
#include <QtWidgets>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QSlider;

class MediaPlayer : public QWidget
{
    Q_OBJECT

public:
    MediaPlayer(QWidget *parent = 0);
    ~MediaPlayer();

    /* Must be called after pointers to UI elements have been assigned */
    void initializeUIElements();

    bool isPlayerAvailable() const;

    void addToPlaylist(const QList<QUrl> urls);

signals:
    void fullScreenChanged(bool fullScreen);

private slots:
    void open();
    void remove();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();

    void previousClicked();

    void seek(int seconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);

    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);

    void displayErrorMessage();

private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);
    void updateDurationInfo(qint64 currentInfo);

public:
    QMediaPlayer* m_player;

    QMediaPlaylist* m_playlist;
    PlaylistModel* m_playlistModel;
    QListView* m_playlistView;

    QVideoWidget* m_videoWidget;

    PlayerControls* m_controls;

    QLabel* m_coverLabel;
    QLabel* m_labelDuration;
    QSlider* m_seekSlider;

    QString m_trackInfo;
    QString m_statusInfo;
    qint64 m_duration;
};

#endif // MEDIAPLAYER_H
