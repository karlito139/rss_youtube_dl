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

#include "video.h"

class RssFeed: public QObject
{
Q_OBJECT

public:
    RssFeed(QString url);

    QList<Video *> *getListVideos(){return listVideos;}


public slots:
    void read();

signals:
    void doneReading();



private:
    void get(QUrl &url);
    void fetch(QString url);
    void parseXml();


    QXmlStreamReader xml;
    QString currentTag;
    QString linkString;
    QString titleString;
    QStringList linkStrings;

    QList<Video *> *listVideos;


    QNetworkReply *currentReply;
    QNetworkAccessManager manager;


};

#endif // RSSFEED_H
