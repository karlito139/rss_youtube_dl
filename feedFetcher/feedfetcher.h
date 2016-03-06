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

#ifndef FEEDFETCHER_H
#define FEEDFETCHER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QUrl>
#include <QUrlQuery>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include "feedfetcheruser.h"

class FeedFetcher: public QObject
{
Q_OBJECT

public:
  FeedFetcher(QSettings *settings, QString clientId, QString clientSecret);
  QList<Video *> *getVideos();

public slots:
    void fetch();

signals:
    void doneFetching();

private slots:
    void userFetched();
    void getNewToken();
    void decodeNewToken(QNetworkReply* reply);

private:
  void getUserVideos();

  FeedFetcherUser *currentUser;

  QSettings *settings;
  QString currentToken;
  QNetworkAccessManager tokenManager;
  int quotaCount;

  QString clientId;
  QString clientSecret;
};

#endif // FEEDFETCHER_H
