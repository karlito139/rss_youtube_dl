#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QRegExp>
#include <QProcess>
#include <QDebug>
#include <QSettings>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


extern QString *pathToFiles;




class Video : public QObject
{
  Q_OBJECT
public:
  explicit Video(QString title, QString link, QSettings *settings, QObject *parent = 0);
  explicit Video(QString id, QSettings *settings, QObject *parent = 0);
  ~Video();
  QString getTitle(){return title;}
  QString getLink(){return link;}
  QString getCode(){return code;}
  QDateTime getReleaseDate()const{return releaseDate;}
  bool haveAlreadyBeenDownloaded(){return alreadyDownloaded;}
  bool isCurrentlyDownloading(){return currentlyDownloading;}
  bool isVideoInitialised(){return haveBeenInitialised;}

  void download();

  bool operator<(const Video &i1) const;
  static bool lessThan(const Video *v1, const Video *v2);


signals:
  void videoDownloaded(Video *);
  void videoDownloadStarted(Video *);
  void videoStatusChanged();
  
public slots:
  void doneDownloading();
  void stopDownload();
  void reset();
  void setAsDownloaded();
  void decodeVideoInfo(QJsonObject reply);

private:

  QString extractCode(QString link);

  QString title;
  QString link;
  QString code;
  bool alreadyDownloaded;
  bool currentlyDownloading;
  QProcess *proc;
  QSettings *settings;
  QDateTime releaseDate;

  QNetworkAccessManager manager;
  bool haveBeenInitialised;
  

};

#endif // VIDEO_H
