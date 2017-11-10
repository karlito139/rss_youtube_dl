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

#ifndef RSSFEED_H
#define RSSFEED_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QXmlStreamReader>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QList>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QUrlQuery>

#include "video.h"


#define MAX_ITEMS_PER_REQUESTS 50



struct PlaylistInfo{

    QString channelID;
    QString playlistID;
};


class RssFeed: public QObject
{
    Q_OBJECT

public:
    RssFeed(QString url, QSettings *settings);
    RssFeed(QSettings *settings);
    ~RssFeed();

    QList<Video *> *getListVideos(){return listVideos;}
    void displayQotaStatus();

public slots:
    void fetch(QString clientId, QString clientSecret);

private slots:
    void read(QNetworkReply *reply);

    void getSubscribedChannelsList();
    void decodeSubscribedChannelsList(QNetworkReply* reply);
    void getPlaylistId(QList<QString> channelIDs);
    void decondePlaylistId(QNetworkReply* reply);
    void getListOfVideos(QString playlistID);
    void decodeListOfVideos(QNetworkReply* reply);
    void decodeVideoInfo(QNetworkReply* reply);
    void getMissingVidInfos();

    void getNewToken(QString clientId, QString clientSecret);
    void decodeNewToken(QNetworkReply* reply);

signals:
    void doneReading();



private:
    void get(QUrl &url);
    void parseXml();
    QString extractCode(QString link);
    bool isInVideoList(QString code);
    void addQuotaUsage(int amount);
    void getVideosInfo(QList<QString> videoList);
    void initPlaylistsInfos();
    void savePlaylistsInfos();

    QXmlStreamReader xml;
    QString currentTag;
    QString currentToken;
    QString linkString;
    QString titleString;
    QStringList linkStrings;
    QSettings *settings;
    QString url;
    QList<Video *> *listVideos;
    QNetworkAccessManager tokenManager;
    QNetworkAccessManager manager;
    QNetworkAccessManager manager2;
    QNetworkAccessManager manager3;
    QNetworkAccessManager manager4;
    QTimer *videoInfoFetchingTimer;
    bool isAlreadyFetching;

    //youtube API V3.0
    int quotaCount;
    QList<PlaylistInfo> playlistInfos;




    QString channelID;


};

#endif // RSSFEED_H
