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

#include "feedfetcherplaylist.h"

FeedFetcherPlaylist::FeedFetcherPlaylist(QString playlistId, QSettings *settings, QString clientId, QString clientSecret)
{
    this->playlistId = playlistId;
    this->settings = settings;
    this->clientId = clientId;
    this->clientSecret = clientSecret;
    this->quotaCount = 0;

    listVideos = new QList<Video *>();
}

QString FeedFetcherPlaylist::getPlaylistId()
{
    return this->playlistId;
}


void FeedFetcherPlaylist::fetch(QString currentToken)
{
    this->currentToken = currentToken;

    getListOfVideos();
}


QList<Video *> *FeedFetcherPlaylist::getVideos()
{
    return listVideos;
}


void FeedFetcherPlaylist::getListOfVideos(QString nextPageTocken)
{
    QString url;
    //static int count = 0;

    url = "https://www.googleapis.com/youtube/v3";
    url += "/playlistItems";

    url += "?part=contentDetails";
    url += "&playlistId=" + playlistId;
    url += "&maxResults=50";

    if( nextPageTocken != NULL )
        url += "&pageToken=" + nextPageTocken;

    url += "&access_token=" + currentToken;

    //qDebug() << url;
    //count++;
    //qDebug() << "Got called for " << playlistID << " : " << QString::number(count);

    QNetworkRequest request(url);
    manager.get(request);
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(decodeListOfVideos(QNetworkReply*)), Qt::UniqueConnection);

    addQuotaUsage(1+2);  // 1 for the request, 2 for the contentDetails
}




void FeedFetcherPlaylist::decodeListOfVideos(QNetworkReply* reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    //qDebug() << "Status code :" << statusCode;

    if (statusCode >= 200 && statusCode < 300)
    {
        QString data = (QString)reply->readAll();

        //qDebug() << "\n\n\nreceived data :" << data;

        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonResponseObj = jsonResponse.object();

        // Ask for the next page if needed
        QString nextPageTocken = jsonResponseObj.value("nextPageToken").toString();

        if( nextPageTocken.isEmpty() == false )
            getListOfVideos( nextPageTocken );

        QJsonArray videoList = jsonResponseObj.value("items").toArray();
        for(int i=0; i<videoList.count(); i++)
        {
            QJsonObject video = videoList.at(i).toObject();

            QString videoID = video.value("contentDetails").toObject().value("videoId").toString();


            //if not already existing
            bool videoExiste = false;
            for(int i=0; i<listVideos->count(); i++)
                if(listVideos->at(i)->getCode() == videoID)
                    videoExiste = true;

            if(videoExiste == false)
            {
                //qDebug() << "Created the video : " << videoID;
                listVideos->append(new Video(videoID, settings));

                /*if(videoInfoFetchingTimer->isActive() == false)
            videoInfoFetchingTimer->start();*/
            }
        }
    }

    emit doneFetching();
}



void FeedFetcherPlaylist::addQuotaUsage(int amount)
{
    quotaCount += amount;
}

int FeedFetcherPlaylist::getQuotaUsed()
{
    int totalQuota = 0;

    totalQuota += quotaCount;

    return totalQuota;
}

