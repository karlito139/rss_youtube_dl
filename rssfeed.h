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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

#include "video.h"


#define MAX_ITEMS_PER_REQUESTS 50


class RssFeed: public QObject
{
Q_OBJECT

public:
    RssFeed(QString url, QSettings *settings, QString apiKey);
    RssFeed(QSettings *settings, QString apiKey);
    ~RssFeed();

    QList<Video *> *getListVideos(){return listVideos;}
    void displayQotaStatus();

public slots:
    void fetch();
    void setURL(QString url);
    void setChannelId(QString id);


private slots:
    void read(QNetworkReply *reply);

    void getSubscribedChannelsList();
    void decodeSubscribedChannelsList(QNetworkReply* reply);
    void getPlaylistId(QList<QString> channelIDs);
    void decondePlaylistId(QNetworkReply* reply);
    void getListOfVideos(QString playlistID);
    void decodeListOfVideos(QNetworkReply* reply);
    void decodeVideoInfo(QNetworkReply* reply);
    void getMissingVidInfos();

signals:
    void doneReading();



private:
    void get(QUrl &url);
    void parseXml();
    QString extractCode(QString link);
    bool isInVideoList(QString code);
    void addQuotaUsage(int amount);
    void getVideosInfo(QList<QString> videoList);

    QXmlStreamReader xml;
    QString currentTag;
    QString linkString;
    QString titleString;
    QString apiKey;
    QStringList linkStrings;
    QSettings *settings;
    QString url;
    QList<Video *> *listVideos;
    QNetworkAccessManager manager;
    QNetworkAccessManager manager2;
    QNetworkAccessManager manager3;
    QNetworkAccessManager manager4;
    QTimer *videoInfoFetchingTimer;

    //youtube API V3.0
    int quotaCount;





    QString channelID;


};

#endif // RSSFEED_H
