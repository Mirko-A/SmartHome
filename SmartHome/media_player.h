#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include "playlist_model.h"
#include "histogram_widget.h"
#include "video_widget.h"

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class QAbstractItemView;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QSlider;
class QVideoProbe;
class QVideoWidget;

class MediaPlayer : public QWidget
{
    Q_OBJECT

public:
    MediaPlayer(QWidget *parent = 0);
    ~MediaPlayer();

    bool isPlayerAvailable() const;

    void addToPlaylist(const QList<QUrl> urls);

signals:
    void fullScreenChanged(bool fullScreen);

private slots:
    void open();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();

    void previousClicked();

    void seek(int seconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);

    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);
    void videoAvailableChanged(bool available);

    void displayErrorMessage();

#ifndef PLAYER_NO_COLOROPTIONS
    void showColorDialog();
#endif

private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);
    void updateDurationInfo(qint64 currentInfo);

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    VideoWidget *videoWidget;
    QLabel *coverLabel;
    QSlider *slider;
    QLabel *labelDuration;
    QPushButton *fullScreenButton;
#ifndef PLAYER_NO_COLOROPTIONS
    QPushButton *colorButton;
    QDialog *colorDialog;
#endif

    QLabel *labelHistogram;
    HistogramWidget *histogram;
    QVideoProbe *probe;

    PlaylistModel *playlistModel;
    QAbstractItemView *playlistView;
    QString trackInfo;
    QString statusInfo;
    qint64 duration;
};

#endif // MEDIAPLAYER_H
