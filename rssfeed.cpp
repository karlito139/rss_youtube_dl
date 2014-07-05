#include "rssfeed.h"

RssFeed::RssFeed(QString url, QSettings *settings) :
  currentReply(0)
{

  this->downloadedVideos = downloadedVideos;
  this->settings = settings;
  this->url = url;

  listVideos = new QList<Video *>();

  fetch();
}

void RssFeed::fetch()
{
    xml.clear();

    QUrl qurl(url);
    get(qurl);
}


void RssFeed::get(QUrl &url)
{
    QNetworkRequest request(url);
    if (currentReply) {
        currentReply->disconnect(this);
        currentReply->deleteLater();
    }
    currentReply = manager.get(request);
    connect(currentReply, SIGNAL(readChannelFinished()), this, SLOT(read()));

    //TODO : code those two next ones
    //connect(currentReply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    //connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}


void RssFeed::read()
{
    int statusCode = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300) {
        QByteArray data = currentReply->readAll();

        // to display the raw text of the RSS feed
        //qDebug() << data;

        xml.addData(data);
        parseXml();

        emit doneReading();
    }
}


void RssFeed::parseXml()
{

  delete listVideos;
  listVideos = new QList<Video *>();

  while (!xml.atEnd()) {
      xml.readNext();
      if (xml.isStartElement()) {
          currentTag = xml.name().toString();
      } else if (xml.isEndElement()) {
          if (xml.name() == "entry") {

              listVideos->append(new Video(titleString, linkString, settings));

              titleString.clear();
              linkString.clear();
          }

      } else if (xml.isCharacters() && !xml.isWhitespace()) {
          if (currentTag == "title")
            titleString += xml.text().toString();
          else if (currentTag == "id")
            linkString += xml.text().toString();
      }
  }
  if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
      //qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
  }
}









