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

#include "rssfeed.h"

RssFeed::RssFeed(QString url, QSettings *settings) :
  QObject()
{
  this->settings = settings;
  this->url = url;

  videoInfoFetchingTimer = new QTimer();
  videoInfoFetchingTimer->setInterval(10*1000);  //10 seconds
  videoInfoFetchingTimer->stop();
  connect(videoInfoFetchingTimer, SIGNAL(timeout()), this, SLOT(getMissingVidInfos()));
  initPlaylistsInfos();

  listVideos = new QList<Video *>();
  quotaCount = 0;
  isAlreadyFetching = false;
}

RssFeed::RssFeed(QSettings *settings) :
  QObject()
{

  this->settings = settings;

  quotaCount = 0;
  isAlreadyFetching = false;
  initPlaylistsInfos();

  videoInfoFetchingTimer = new QTimer();
  videoInfoFetchingTimer->setInterval(10*1000);  //10 seconds
  videoInfoFetchingTimer->stop();
  connect(videoInfoFetchingTimer, SIGNAL(timeout()), this, SLOT(getMissingVidInfos()));

  listVideos = new QList<Video *>();
}

RssFeed::~RssFeed()
{
  qDeleteAll(*listVideos);
  listVideos->clear();
  delete listVideos;
  delete videoInfoFetchingTimer;
}


void RssFeed::fetch(QString clientId, QString clientSecret)
{
  getNewToken(clientId, clientSecret);
}


void RssFeed::initPlaylistsInfos()
{
  int size = settings->beginReadArray("playlists");
  for (int i = 0; i < size; ++i) {
    settings->setArrayIndex(i);
    PlaylistInfo playlist;

    playlist.channelID = settings->value("channel").toString();
    playlist.playlistID = settings->value("playlist").toString();

    playlistInfos.append(playlist);
  }
  settings->endArray();
}


void RssFeed::savePlaylistsInfos()
{
  settings->beginWriteArray("playlists");
  for(int i=0; i<playlistInfos.count(); i++)
  {
    settings->setArrayIndex(i);
    settings->setValue("channel", playlistInfos.at(i).channelID);
    settings->setValue("playlist", playlistInfos.at(i).playlistID);
  }

  settings->endArray();
  settings->sync();
}



void RssFeed::getNewToken(QString clientId, QString clientSecret)
{
  QString url = "https://www.googleapis.com/oauth2/v4/token";
  QUrlQuery postData;
  postData.addQueryItem("client_id", clientId);
  postData.addQueryItem("client_secret", clientSecret);
  postData.addQueryItem("refresh_token", settings->value("refreshToken", "").toString());
  postData.addQueryItem("grant_type", "refresh_token");

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  tokenManager.post(request, postData.toString(QUrl::FullyEncoded).toUtf8());

  //qDebug() << "Ask for a new tocken";

  connect(&tokenManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(decodeNewToken(QNetworkReply*)));
}




void RssFeed::decodeNewToken(QNetworkReply* reply)
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
        currentToken = token;

      //qDebug() << "new tocken : " << currentToken;

      getSubscribedChannelsList();
  }
}






void RssFeed::getSubscribedChannelsList()
{
    QString url;

    if(isAlreadyFetching == false)
    {
      isAlreadyFetching = true;

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
}


void RssFeed::decodeSubscribedChannelsList(QNetworkReply* reply)
{
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  //qDebug() << "statusCode : " << statusCode;
  isAlreadyFetching = false;

  if (statusCode >= 200 && statusCode < 300) {
      QString data = (QString)reply->readAll();

      //qDebug() << "list of subscriptions : " << data;

      QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
      QJsonObject jsonResponseObj = jsonResponse.object();


      QJsonArray channelList = jsonResponseObj.value("items").toArray();
      QList<QString> channelsToFetch;
      for(int i=0; i<channelList.count(); i++)
      {
        QJsonObject channel = channelList.at(i).toObject();

        QString channelName = channel.value("snippet").toObject().value("title").toString();
        QString channelID = channel.value("snippet").toObject().value("resourceId").toObject().value("channelId").toString();
        //qDebug() << "Channel " << channelName << "'s ID is : " << channelID;


        bool isAlreadyFetched = false;
        QString playlistId;

        //Seach in the current config if we didn't already fetch the right playlist ID for this channel
        for(int j=0; j<playlistInfos.size(); j++)
        {
          if(playlistInfos.at(j).channelID == channelID)
          {
            playlistId = playlistInfos.at(j).playlistID;
            isAlreadyFetched = true;
          }
        }

        if(isAlreadyFetched == false)
          channelsToFetch.append(channelID);
        else
          getListOfVideos(playlistId);
      }

      //qDebug() << "I need to fetch : " << channelsToFetch.count() << "playlits";

      if(channelsToFetch.count() > 0)
        getPlaylistId(channelsToFetch);
  }
}






void RssFeed::getPlaylistId(QList<QString> channelIDs)
{
  QString url;

  url = "https://www.googleapis.com/youtube/v3";
  url += "/channels";

  url += "?part=contentDetails";
  url += "&id=";
  for(int i=0; i<channelIDs.count(); i++)
    url += channelIDs.at(i) + ",";
  url = url.left(url.size()-1);

  url += "&maxResults=50";
  url += "&access_token=" + currentToken;

  //qDebug() << url;

  QNetworkRequest request(url);
  manager2.get(request);
  connect(&manager2, SIGNAL(finished(QNetworkReply*)), this, SLOT(decondePlaylistId(QNetworkReply*)));

  addQuotaUsage(1+2);  // 1 for the request, 2 for the contentDetails
}



void RssFeed::decondePlaylistId(QNetworkReply* reply)
{
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  if (statusCode >= 200 && statusCode < 300) {
      QString data = (QString)reply->readAll();

      //qDebug() << "received data :" << data;

      QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
      QJsonObject jsonResponseObj = jsonResponse.object();

      QJsonArray playlistList = jsonResponseObj.value("items").toArray();

      for(int i=0; i<playlistList.count(); i++)
      {
        QJsonObject playlist = playlistList.at(i).toObject();

        QString channelID = playlist.value("id").toString();
        QString playlistID = playlist.value("contentDetails").toObject().value("relatedPlaylists").toObject().value("uploads").toString();

        //qDebug() << "Ask about the playlist : " << playlistID;
        getListOfVideos(playlistID);

        PlaylistInfo playlistInfo;
        playlistInfo.channelID = channelID;
        playlistInfo.playlistID = playlistID;
        playlistInfos.append(playlistInfo);
      }

      savePlaylistsInfos();
  }
}












void RssFeed::getListOfVideos(QString playlistID)
{
  QString url;
  //static int count = 0;

  url = "https://www.googleapis.com/youtube/v3";
  url += "/playlistItems";

  url += "?part=contentDetails";
  url += "&playlistId=" + playlistID;
  url += "&maxResults=5";
  url += "&access_token=" + currentToken;

  //qDebug() << url;
  //count++;
  //qDebug() << "Got called for " << playlistID << " : " << QString::number(count);

  QNetworkRequest request(url);
  manager3.get(request);
  connect(&manager3, SIGNAL(finished(QNetworkReply*)), this, SLOT(decodeListOfVideos(QNetworkReply*)), Qt::UniqueConnection);

  addQuotaUsage(1+2);  // 1 for the request, 2 for the contentDetails
}




void RssFeed::decodeListOfVideos(QNetworkReply* reply)
{
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  //qDebug() << "Status code :" << statusCode;

  if (statusCode >= 200 && statusCode < 300) {
      QString data = (QString)reply->readAll();

      //qDebug() << "\n\n\nreceived data :" << data;

      QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
      QJsonObject jsonResponseObj = jsonResponse.object();

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

          if(videoInfoFetchingTimer->isActive() == false)
            videoInfoFetchingTimer->start();

          emit doneReading();
        }
      }
  }
}


void RssFeed::getMissingVidInfos()
{
  QList<QString> videosToFetch;

  videoInfoFetchingTimer->stop();

  for(int i=0; i<listVideos->count(); i++)
  {
    if(listVideos->at(i)->isVideoInitialised() == false)
    {
      videosToFetch.append(listVideos->at(i)->getCode());
    }
  }

  getVideosInfo(videosToFetch);
}


void RssFeed::getVideosInfo(QList<QString> videoList)
{
  //we need to make requests of 50 videos at a time else youtube refuses to process it

  for(int i=0; i<((videoList.count()/MAX_ITEMS_PER_REQUESTS)+1); i++)
  {
    QString url;
    url = "https://www.googleapis.com/youtube/v3";
    url += "/videos";

    url += "?part=snippet";
    url += "&id=";

    for(int j=0; ((((i*MAX_ITEMS_PER_REQUESTS)+j)<videoList.count()) && (j<MAX_ITEMS_PER_REQUESTS)); j++)
      url += videoList.at((i*MAX_ITEMS_PER_REQUESTS)+j) + ",";


    url = url.left(url.size()-1);

    url += "&maxResults=50";
    url += "&access_token=" + currentToken;

    QNetworkRequest request(url);
    manager4.get(request);
    connect(&manager4, SIGNAL(finished(QNetworkReply*)), this, SLOT(decodeVideoInfo(QNetworkReply*)), Qt::UniqueConnection);
    addQuotaUsage(1+2);  // 1 for the request, 2 for the snippet
  }
}


void RssFeed::decodeVideoInfo(QNetworkReply* reply)
{
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  if (statusCode >= 200 && statusCode < 300) {
      QString data = (QString)reply->readAll();

      QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
      QJsonObject jsonResponseObj = jsonResponse.object();

      QJsonArray videoList = jsonResponseObj.value("items").toArray();
      for(int i=0; i<videoList.count(); i++)
      {
        QJsonObject video = videoList.at(i).toObject();
        QString videoID = video.value("id").toString();

        for(int i=0; i<listVideos->count(); i++)
          if(listVideos->at(i)->getCode() == videoID)
            listVideos->at(i)->decodeVideoInfo(video.value("snippet").toObject());
      }
  }

  displayQotaStatus();
}


















void RssFeed::addQuotaUsage(int amount)
{
  quotaCount += amount;
}




void RssFeed::displayQotaStatus()
{
  qDebug() << "We now got : " << listVideos->count() << " videos. That costed : " << QString::number(quotaCount) << " of youtube cota.";
}





































void RssFeed::get(QUrl &url)
{
    QNetworkRequest request(url);
    manager.get(request);
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(read(QNetworkReply*)));

    //TODO : code those two next ones
    //connect(currentReply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    //connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}


void RssFeed::read(QNetworkReply* reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (statusCode >= 200 && statusCode < 300) {
        QByteArray data = reply->readAll();

        xml.addData(data);
        parseXml();

        emit doneReading();
    }
}


QString RssFeed::extractCode(QString link){

  QRegExp regExp("videos/.*");

  regExp.indexIn(link, 0);

  return regExp.cap(0).mid(7,-1);
}

bool RssFeed::isInVideoList(QString code){

  for(int i=0; i<listVideos->count(); i++){

      if(listVideos->at(i)->getCode() == code)     return true;
  }

  return false;
}



void RssFeed::parseXml()
{

  while (!xml.atEnd()) {
      xml.readNext();
      if (xml.isStartElement()) {
          currentTag = xml.name().toString();
      } else if (xml.isEndElement()) {
          if (xml.name() == "entry") {

              QString code = extractCode(linkString);

              if(!isInVideoList(code)){ //code video is not already in list

                  listVideos->append(new Video(titleString, code, settings));
              }

              titleString.clear();
              linkString.clear();
          }

      } else if (xml.isCharacters() && !xml.isWhitespace()) {
          if (currentTag == "title") {

            if(!xml.text().toString().contains("New Subscription Videos")) titleString += xml.text().toString();
          }else if (currentTag == "id"){

            linkString += xml.text().toString();
          }
      }
  }
  if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
      //qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
  }
}









