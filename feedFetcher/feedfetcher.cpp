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

#include "feedfetcher.h"
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>


FeedFetcher::FeedFetcher(QSettings *settings, QString clientId, QString clientSecret)
{
    this->settings = settings;
    this->clientId = clientId;
    this->clientSecret = clientSecret;
    quotaCount = 0;

    currentUser = new FeedFetcherUser(settings, clientId, clientSecret);
    connect(currentUser, SIGNAL(doneFetching()), this, SLOT(userFetched()));
}

void FeedFetcher::fetch()
{
    getNewToken();
}

QList<Video *> *FeedFetcher::getVideos()
{
    return currentUser->getVideos();
}

void FeedFetcher::getNewToken()
{
    QString url = "https://oauth2.googleapis.com/token";
    QUrlQuery postData;
    postData.addQueryItem("client_id", clientId);
    postData.addQueryItem("client_secret", clientSecret);
    postData.addQueryItem("grant_type", "refresh_token");
    postData.addQueryItem("refresh_token", settings->value("refreshToken", "").toString());
    postData.addQueryItem("access_type", "offline");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    tokenManager.post(request, postData.toString(QUrl::FullyEncoded).toUtf8());

    //qDebug() << "Ask for a new tocken";

    connect(&tokenManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(decodeNewToken(QNetworkReply*)), Qt::UniqueConnection);
}

void FeedFetcher::decodeNewToken(QNetworkReply* reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    //qDebug() << "statusCode : " << statusCode;

    if (statusCode >= 200 && statusCode < 300) {
        QString data = (QString)reply->readAll();

        //qDebug() << "Received data : " << data;

        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonResponseObj = jsonResponse.object();

        QString token = jsonResponseObj.value("access_token").toString();

        if(!token.isEmpty())
        {
            currentToken = token;
            settings->setValue("refreshToken", token);
            settings->sync();
        }

        //qDebug() << "new token : " << currentToken;

        getUserVideos();
    }
    else if(statusCode == 400)
    {
        // We need to refresh the token
        QString data = (QString)reply->readAll();

        qDebug() << "new token error : " << data;
    }

    reply->deleteLater();
}

void FeedFetcher::getUserVideos()
{
    //qDebug() << "Request to fetch user.";
    currentUser->fetch(currentToken);
}

void FeedFetcher::userFetched()
{
    emit doneFetching();

    displayQotaStatus();
}

void FeedFetcher::displayQotaStatus()
{

    qDebug() << "We now got : " << getVideos()->count() << " videos. That costed : " << QString::number(currentUser->getQuotaUsed()) << " of youtube cota.";
}


