#include "rssfeed.h"

RssFeed::RssFeed(QString url) :
  currentReply(0)
{
  listVideos = new QList<Video *>();

  fetch(url);

}

void RssFeed::fetch(QString url)
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
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            currentTag = xml.name().toString();
        } else if (xml.isEndElement()) {
            if (xml.name() == "entry") {

                //qDebug() << "kikoo : " << titleString << "\n\n url : " << linkString << "\n\r";

                listVideos->append(new Video(titleString, linkString));

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









