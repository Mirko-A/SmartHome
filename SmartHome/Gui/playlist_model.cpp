#include "playlist_model.h"

#include <QFileInfo>
#include <QMediaPlaylist>
#include <QUrl>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractItemModel(parent), m_playlist(nullptr), m_openButton(nullptr),
      m_removeButton(nullptr) {}

int PlaylistModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return (m_playlist != nullptr) ? m_playlist->mediaCount() : 0U;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return ColumnCount;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return ((m_playlist != nullptr) && (row >= 0 && row < m_playlist->mediaCount()) &&
            (column >= 0 && column < ColumnCount))
               ? createIndex(row, column)
               : QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const {
    Q_UNUSED(child);

    return QModelIndex();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const {
    QVariant result;

    if (index.isValid() && role == Qt::DisplayRole) {
        result = m_data[index];
        if (!result.isValid() && index.column() == Title) {
            QUrl location = m_playlist->media(index.row()).request().url();
            result = QFileInfo(location.path()).fileName();
        }
    }

    return result;
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    Q_UNUSED(role);

    bool result = false;

    if (index.isValid()) {
        m_data[index] = value;
        result = true;
        emit dataChanged(index, index);
    }

    return result;
}

QMediaPlaylist *PlaylistModel::playlist() const {
    return m_playlist;
}

void PlaylistModel::setPlaylist(QMediaPlaylist *playlist) {
    if (m_playlist) {
        disconnect(m_playlist, &QMediaPlaylist::mediaAboutToBeInserted, this,
                   &PlaylistModel::beginInsertItems);
        disconnect(m_playlist, &QMediaPlaylist::mediaInserted, this,
                   &PlaylistModel::endInsertItems);
        disconnect(m_playlist, &QMediaPlaylist::mediaAboutToBeRemoved, this,
                   &PlaylistModel::beginRemoveItems);
        disconnect(m_playlist, &QMediaPlaylist::mediaRemoved, this, &PlaylistModel::endRemoveItems);
        disconnect(m_playlist, &QMediaPlaylist::mediaChanged, this, &PlaylistModel::changeItems);
    }

    beginResetModel();
    m_playlist = playlist;

    if (m_playlist) {
        connect(m_playlist, &QMediaPlaylist::mediaAboutToBeInserted, this,
                &PlaylistModel::beginInsertItems);
        connect(m_playlist, &QMediaPlaylist::mediaInserted, this, &PlaylistModel::endInsertItems);
        connect(m_playlist, &QMediaPlaylist::mediaAboutToBeRemoved, this,
                &PlaylistModel::beginRemoveItems);
        connect(m_playlist, &QMediaPlaylist::mediaRemoved, this, &PlaylistModel::endRemoveItems);
        connect(m_playlist, &QMediaPlaylist::mediaChanged, this, &PlaylistModel::changeItems);
    }

    endResetModel();
}

void PlaylistModel::beginInsertItems(int start, int end) {
    m_data.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::endInsertItems() {
    endInsertRows();
}

void PlaylistModel::beginRemoveItems(int start, int end) {
    m_data.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::endRemoveItems() {
    endRemoveRows();
}

void PlaylistModel::changeItems(int start, int end) {
    m_data.clear();
    emit dataChanged(index(start, 0), index(end, ColumnCount));
}
