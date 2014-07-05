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
  explicit Video(QString title, QString link, QSettings *settings, QStringList *downloadedVideos, QObject *parent = 0);
  QString getTitle(){return title;}
  QString getLink(){return link;}
  QString getCode(){return code;}
  bool haveAlreadyBeenDownloaded(){return alreadyDownloaded;}

  void download();


signals:
  void videoDownloaded(QString code);
  
public slots:
  void doneDownloading();


private:

  QString extractCode(QString link);

  QString title;
  QString link;
  QString code;
  bool alreadyDownloaded;
  QProcess *proc;
  QSettings *settings;



  
};

#endif // VIDEO_H
