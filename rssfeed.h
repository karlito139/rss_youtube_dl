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

#include "video.h"

class RssFeed: public QObject
{
Q_OBJECT

public:
    RssFeed(QString url, QSettings *settings);
    RssFeed(QSettings *settings);
    ~RssFeed();

    QList<Video *> *getListVideos(){return listVideos;}


public slots:
    void fetch();
    void setURL(QString url);


private slots:
    void read(QNetworkReply *reply);


signals:
    void doneReading();



private:
    void get(QUrl &url);
    void parseXml();
    QString extractCode(QString link);
    bool isInVideoList(QString code);

    QXmlStreamReader xml;
    QString currentTag;
    QString linkString;
    QString titleString;
    QStringList linkStrings;
    QSettings *settings;
    QString url;
    QList<Video *> *listVideos;
    QNetworkAccessManager manager;


};

#endif // RSSFEED_H
