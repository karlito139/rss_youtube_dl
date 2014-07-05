#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QRegExp>
#include <QProcess>
#include <QDebug>
#include <QSettings>


class Video : public QObject
{
  Q_OBJECT
public:
  explicit Video(QString title, QString link, QSettings *settings, QObject *parent = 0);
  QString getTitle(){return title;}
  QString getLink(){return link;}
  QString getCode(){return code;}
  bool haveAlreadyBeenDownloaded(){return alreadyDownloaded;}
  bool isCurrentlyDownloading(){return currentlyDownloading;}

  void download();


signals:
  void videoDownloaded(Video *);
  void videoDownloadStarted(Video *);
  
public slots:
  void doneDownloading();
  void stopDownload();


private:

  QString extractCode(QString link);

  QString title;
  QString link;
  QString code;
  bool alreadyDownloaded;
  bool currentlyDownloading;
  QProcess *proc;
  QSettings *settings;



  
};

#endif // VIDEO_H
