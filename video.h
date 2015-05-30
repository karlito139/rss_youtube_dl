/*
Copyright (c) 2015 Clement Roblot

This file is part of localtube.

Localtube is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Localtube is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with localtube.  If not, see <http://www.gnu.org/licenses/>.
*/

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
