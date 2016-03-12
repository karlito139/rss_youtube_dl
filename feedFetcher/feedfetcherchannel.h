#ifndef FEEDFETCHERCHANNEL_H
#define FEEDFETCHERCHANNEL_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QUrl>
#include <QUrlQuery>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "feedfetcherplaylist.h"

struct PlaylistInfo{

  QString channelID;
  QString playlistID;
};

class FeedFetcherChannel: public QObject
{
Q_OBJECT

public:
  FeedFetcherChannel(QString channelName, QString channelID, QSettings *settings, QString clientId, QString clientSecret);
  QString getChannelId();
  QList<Video *> *getVideos();
  int getQuotaUsed();

public slots:
  void fetch(QString currentToken);

signals:
    void doneFetching();

private slots:
    void playlistFetched();
    void getPlaylistId(QList<QString> channelIDs);
    void decondePlaylistId(QNetworkReply* reply);

private:
  void initPlaylistsInfos();
  void savePlaylistsInfos();
  void addPlayList(QString playlistId);

  void addQuotaUsage(int amount);


  QString channelName;
  QString channelID;
  QSettings *settings;
  QString clientId;
  QString clientSecret;
  QString currentToken;
  QList<PlaylistInfo> playlistInfos;
  QNetworkAccessManager manager;
  int quotaCount;

  QList<FeedFetcherPlaylist *> *playlistList;

};

#endif // FEEDFETCHERCHANNEL_H
