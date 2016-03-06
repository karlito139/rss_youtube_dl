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

#include "feedfetcherchannel.h"

class FeedFetcherUser: public QObject
{
Q_OBJECT

public:
  FeedFetcherUser(QSettings *settings, QString clientId, QString clientSecret);
  QList<Video *> *getVideos();

public slots:
  void fetch(QString currentToken);

signals:
    void doneFetching();

private slots:
    void channelFetched();
    void getSubscribedChannelsList();
    void decodeSubscribedChannelsList(QNetworkReply* reply);

private:
  void addQuotaUsage(int amount);

  QSettings *settings;
  QString clientId;
  QString clientSecret;
  QString currentToken;
  QNetworkAccessManager manager;
  QList<FeedFetcherChannel *> *channelList;

  int quotaCount;

};

#endif // FEEDFETCHERUSER_H
