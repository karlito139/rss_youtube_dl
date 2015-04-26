#include "rssfeed.h"

RssFeed::RssFeed(QString url, QSettings *settings, QString apiKey) :
  QObject()
{
  this->settings = settings;
  this->url = url;
  this->apiKey = apiKey;

  videoInfoFetchingTimer = new QTimer();
  videoInfoFetchingTimer->setInterval(10*1000);  //10 seconds
  videoInfoFetchingTimer->stop();
  connect(videoInfoFetchingTimer, SIGNAL(timeout()), this, SLOT(getMissingVidInfos()));

  listVideos = new QList<Video *>();
  quotaCount = 0;

  fetch();
}

RssFeed::RssFeed(QSettings *settings, QString apiKey) :
  QObject()
{

  this->settings = settings;
  this->apiKey = apiKey;

  quotaCount = 0;

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

void RssFeed::setURL(QString url){

  this->url = url;
  fetch();
}


void RssFeed::setChannelId(QString id)
{
  this->channelID = id;
  fetch();
}



void RssFeed::fetch()
{
  getSubscribedChannelsList();
}








void RssFeed::getSubscribedChannelsList()
{
    QString url;

    url = "https://www.googleapis.com/youtube/v3";
    url += "/subscriptions";

    url += "?part=snippet";
    url += "&channelId=" + this->channelID;
    url += "&maxResults=50";
    url += "&key=" + this->apiKey;

    QNetworkRequest request(url);
    manager.get(request);
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(decodeSubscribedChannelsList(QNetworkReply*)));

    addQuotaUsage(1+2);  // 1 for the request, 2 for the snippet
}


void RssFeed::decodeSubscribedChannelsList(QNetworkReply* reply)
{
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  if (statusCode >= 200 && statusCode < 300) {
      QString data = (QString)reply->readAll();

      QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
      QJsonObject jsonResponseObj = jsonResponse.object();


      QJsonArray channelList = jsonResponseObj.value("items").toArray();
      QList<QString> listOfChannels;
      for(int i=0; i<channelList.count(); i++)
      {
        QJsonObject channel = channelList.at(i).toObject();

        //QString channelName = channel.value("snippet").toObject().value("title").toString();
        QString channelID = channel.value("snippet").toObject().value("resourceId").toObject().value("channelId").toString();
        //qDebug() << "Channel : " << channelName << " it's ID is : " << channelID;

        listOfChannels.append(channelID);
      }

      if(listOfChannels.count() > 0)
        getPlaylistId(listOfChannels);
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
  url += "&key=" + this->apiKey;

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

        QString playlistID = playlist.value("contentDetails").toObject().value("relatedPlaylists").toObject().value("uploads").toString();

        //qDebug() << "Ask about the playlist : " << playlistID;
        getListOfVideos(playlistID);
      }
  }
}












void RssFeed::getListOfVideos(QString playlistID)
{
  QString url;

  url = "https://www.googleapis.com/youtube/v3";
  url += "/playlistItems";

  url += "?part=contentDetails";
  url += "&playlistId=" + playlistID;
  url += "&maxResults=5";
  url += "&key=" + this->apiKey;

  //qDebug() << url;

  QNetworkRequest request(url);
  manager3.get(request);
  connect(&manager3, SIGNAL(finished(QNetworkReply*)), this, SLOT(decodeListOfVideos(QNetworkReply*)), Qt::UniqueConnection);

  addQuotaUsage(1+2);  // 1 for the request, 2 for the contentDetails
}




void RssFeed::decodeListOfVideos(QNetworkReply* reply)
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
    url += "&key=" + this->apiKey;

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
  qDebug() << "We now got : " << listVideos->count() << " videos. That costed : " << QString::number(quotaCount) << " of cota.";
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









