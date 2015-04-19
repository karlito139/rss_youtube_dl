#include "rssfeed.h"

RssFeed::RssFeed(QString url, QSettings *settings) :
  QObject()
{
  this->settings = settings;
  this->url = url;

  listVideos = new QList<Video *>();

  fetch();
}

RssFeed::RssFeed(QSettings *settings) :
  QObject()
{

  this->settings = settings;

  listVideos = new QList<Video *>();
}

RssFeed::~RssFeed()
{
  delete listVideos;
}

void RssFeed::fetch()
{
    xml.clear();

    QUrl qurl(url);
    get(qurl);
}



void RssFeed::setURL(QString url){

  this->url = url;
  fetch();
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









