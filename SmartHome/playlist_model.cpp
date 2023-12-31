#include "playlist_model.h"

#include <QFileInfo>
#include <QUrl>
#include <QMediaPlaylist>

PlaylistModel::PlaylistModel(QObject* parent)
    : QAbstractItemModel(parent),
      m_playlist(nullptr),
      m_openButton(nullptr),
      m_removeButton(nullptr)
{
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return (m_playlist != nullptr) ? m_playlist->mediaCount() : 0U;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return ColumnCount;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return ( (m_playlist != nullptr) &&
             (row >= 0    && row < m_playlist->mediaCount()) &&
             (column >= 0 && column < ColumnCount) ) ? createIndex(row, column) : QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    QVariant result;

    if (index.isValid() && role == Qt::DisplayRole)
    {
        result = m_data[index];
        if (!result.isValid() && index.column() == Title)
        {
            QUrl location = m_playlist->media(index.row()).canonicalUrl();
            result = QFileInfo(location.path()).fileName();
        }
    }

    return result;
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    bool result = false;

    if (index.isValid())
    {
        m_data[index] = value;
        result = true;
        emit dataChanged(index, index);
    }

    return result;
}

QMediaPlaylist *PlaylistModel::playlist() const
{
    return m_playlist;
}

void PlaylistModel::setPlaylist(QMediaPlaylist *playlist)
{
    if (m_playlist)
    {
        disconnect(m_playlist, SIGNAL(mediaAboutToBeInserted(int,int)), this, SLOT(beginInsertItems(int,int)));
        disconnect(m_playlist, SIGNAL(mediaInserted(int,int)), this, SLOT(endInsertItems()));
        disconnect(m_playlist, SIGNAL(mediaAboutToBeRemoved(int,int)), this, SLOT(beginRemoveItems(int,int)));
        disconnect(m_playlist, SIGNAL(mediaRemoved(int,int)), this, SLOT(endRemoveItems()));
        disconnect(m_playlist, SIGNAL(mediaChanged(int,int)), this, SLOT(changeItems(int,int)));
    }

    beginResetModel();
    m_playlist = playlist;

    if (m_playlist) {
        connect(m_playlist, SIGNAL(mediaAboutToBeInserted(int,int)), this, SLOT(beginInsertItems(int,int)));
        connect(m_playlist, SIGNAL(mediaInserted(int,int)), this, SLOT(endInsertItems()));
        connect(m_playlist, SIGNAL(mediaAboutToBeRemoved(int,int)), this, SLOT(beginRemoveItems(int,int)));
        connect(m_playlist, SIGNAL(mediaRemoved(int,int)), this, SLOT(endRemoveItems()));
        connect(m_playlist, SIGNAL(mediaChanged(int,int)), this, SLOT(changeItems(int,int)));
    }

    endResetModel();
}

void PlaylistModel::beginInsertItems(int start, int end)
{
    m_data.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::endInsertItems()
{
    endInsertRows();
}

void PlaylistModel::beginRemoveItems(int start, int end)
{
    m_data.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::endRemoveItems()
{
    endRemoveRows();
}

void PlaylistModel::changeItems(int start, int end)
{
    m_data.clear();
    emit dataChanged(index(start,0), index(end,ColumnCount));
}
