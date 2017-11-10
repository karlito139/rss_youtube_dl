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

#ifndef FEEDFETCHERPLAYLIST_H
#define FEEDFETCHERPLAYLIST_H

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

#include "video.h"

class FeedFetcherPlaylist: public QObject
{
    Q_OBJECT

public:
    FeedFetcherPlaylist(QString playlistId, QSettings *settings, QString clientId, QString clientSecret);
    QString getPlaylistId();
    QList<Video *> *getVideos();
    int getQuotaUsed();

public slots:
    void fetch(QString currentToken);

signals:
    void doneFetching();

private slots:
    void getListOfVideos();
    void decodeListOfVideos(QNetworkReply* reply);

private:
    void addQuotaUsage(int amount);

    QString playlistId;
    QSettings *settings;
    QString clientId;
    QString clientSecret;
    QString currentToken;
    QList<Video *> *listVideos;
    QNetworkAccessManager manager;
    int quotaCount;

};

#endif // FEEDFETCHERPLAYLIST_H
