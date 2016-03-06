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

#include "feedfetcheruser.h"

FeedFetcherUser::FeedFetcherUser(QSettings *settings, QString clientId, QString clientSecret)
{
  this->settings = settings;
  this->clientId = clientId;
  this->clientSecret = clientSecret;
  quotaCount = 0;

  channelList = new QList<FeedFetcherChannel *>();
}

void FeedFetcherUser::fetch(QString currentToken)
{
  this->currentToken = currentToken;
  getSubscribedChannelsList();
}

QList<Video *> *FeedFetcherUser::getVideos()
{
  QList<Video *> *videoList = new QList<Video *>();

  for(int i=0; i<channelList->count(); i++)
  {
    videoList->append(*(channelList->at(i)->getVideos()));
  }

  return videoList;
}

void FeedFetcherUser::getSubscribedChannelsList()
{
    QString url;

    url = "https://www.googleapis.com/youtube/v3";
    url += "/subscriptions";

    url += "?part=snippet";
    url += "&mine=true";
    url += "&maxResults=50";
    url += "&access_token=" + currentToken;

    //qDebug() << url;

    QNetworkRequest request(url);
    manager.get(request);
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(decodeSubscribedChannelsList(QNetworkReply*)));

    addQuotaUsage(1+2);  // 1 for the request, 2 for the snippet
}

void FeedFetcherUser::decodeSubscribedChannelsList(QNetworkReply* reply)
{
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  //qDebug() << "statusCode : " << statusCode;

  if (statusCode >= 200 && statusCode < 300)
  {
      QString data = (QString)reply->readAll();

      //qDebug() << "list of subscriptions : " << data;

      QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
      QJsonObject jsonResponseObj = jsonResponse.object();


      QJsonArray fetchList = jsonResponseObj.value("items").toArray();
      QList<QString> channelsToFetch;
      for(int i=0; i<fetchList.count(); i++)
      {
        QJsonObject channel = fetchList.at(i).toObject();

        QString channelName = channel.value("snippet").toObject().value("title").toString();
        QString channelID = channel.value("snippet").toObject().value("resourceId").toObject().value("channelId").toString();
        //qDebug() << "Channel " << channelName << "'s ID is : " << channelID;


        FeedFetcherChannel *newChannel = new FeedFetcherChannel(channelName, channelID, settings, clientId, clientSecret);
        newChannel->fetch(currentToken);
        connect(newChannel, SIGNAL(doneFetching()), this, SLOT(channelFetched()));
        channelList->append(newChannel);
    }
  }
}

void FeedFetcherUser::channelFetched()
{
  emit doneFetching();
}

void FeedFetcherUser::addQuotaUsage(int amount)
{
  quotaCount += amount;
}

