/*
Copyright (c) 2015 Clement Roblot

This file is part of localtube.

Localtube is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Localtube is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with localtube.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FEEDFETCHERUSER_H
#define FEEDFETCHERUSER_H

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
#include <QTimer>

#include "feedfetcherchannel.h"


#define MAX_ITEMS_PER_REQUESTS 50


class FeedFetcherUser: public QObject
{
Q_OBJECT

public:
  FeedFetcherUser(QSettings *settings, QString clientId, QString clientSecret);
  QList<Video *> *getVideos();
  int getQuotaUsed();

public slots:
  void fetch(QString currentToken);

signals:
    void doneFetching();

private slots:
    void channelFetched();
    void getSubscribedChannelsList(QString nextPageTocken = NULL);
    void decodeSubscribedChannelsList(QNetworkReply* reply);
    void getVideosInfo(QList<Video *> videoList);
    void decodeVideoInfo(QNetworkReply* reply);
    void getMissingVidInfos();
    void getMissingVidInfosForce();

private:
  QList<Video *> getVideosMissingInfos();
  void addQuotaUsage(int amount);

  QSettings *settings;
  QString clientId;
  QString clientSecret;
  QString currentToken;
  QNetworkAccessManager manager;
  QNetworkAccessManager manager2;
  QList<FeedFetcherChannel *> *channelList;
  QTimer *videoInfoFetchingTimer;

  int quotaCount;

};

#endif // FEEDFETCHERUSER_H
