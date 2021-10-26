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

#include "video.h"

Video::Video(QString title, QString link, QSettings *settings, QObject *parent) :
    QObject(parent)
{

    this->title = title;
    this->link = link;
    //this->code = extractCode(link);
    this->code = link;
    this->settings = settings;
    this->proc = NULL;
    this->haveBeenInitialised = true;
    this->isBeingInitialised = false;

    QString videoDownloaded = settings->value("downloaded", "").toString();

    if( videoDownloaded.split("/").contains(code) == true )
    {
        this->status = videoDoneDownloaded;
    }
    else
    {
        this->status = videoNotDownloaded;
    }
}


//Using youtuve API V3.0
Video::Video(QString id, QSettings *settings, QObject *parent) :
    QObject(parent)
{
    this->link = id;
    this->code = id;
    this->title = "Fetching...";
    this->settings = settings;
    this->proc = NULL;
    this->haveBeenInitialised = false;
    this->isBeingInitialised = false;

    QString videoDownloaded = settings->value("downloaded", "").toString();

    if( videoDownloaded.split("/").contains(code) == true )
    {
        this->status = videoDoneDownloaded;
    }
    else
    {
        this->status = videoNotDownloaded;
    }
}

Video::~Video(){

    if(proc != NULL)
    {
        if(this->status == videoDownloading)
            stopDownload();

        delete proc;
    }
}

QString Video::extractCode(QString link){

    QRegExp regExp("videos/.*");

    regExp.indexIn(link, 0);

    return regExp.cap(0).mid(7,-1);
}


bool Video::download(){

    QString destination = settings->value("destination", "").toString();

    //If the storage is not valid, we don't download
    QStorageInfo storage(destination);
    if (storage.isValid() == false)
        return false;

    float DiskLimit = settings->value("disk_limit", 0).toFloat();   //In Go
    DiskLimit = DiskLimit * 1000; //In Mo
    float diskSpace = (storage.bytesAvailable()/(1000*1000)); //In Mo

    //qDebug() << "Trying to put a video in a disk with : " << QString::number(diskSpace) << "Mo left. Limit at : " << QString::number(DiskLimit);

    //If there is not enough free space, we don't download
    if( diskSpace < DiskLimit )
    {
        this->status = videoError;

        return false;
    }

    if( (this->status == videoNotDownloaded) || (this->status == videoError) )
    {
        /* create QProcess object */
        proc= new QProcess();

        QStringList arguments;

#ifdef  Q_OS_LINUX
        //arguments << "-f" << "bestvideo+bestaudio/best";
        //arguments << "-f" << "best";
#else
        arguments << "-f" << "best";
#endif
        arguments << "-o" << settings->value("destination", "").toString() + "[%(upload_date)s]%(channel)s_%(title)s.%(ext)s";
        arguments << "-f" << "bestvideo+bestaudio";
        arguments << this->code;

        //qDebug() << "Downloading with args : " << arguments;

#ifdef  Q_OS_LINUX
        proc->start(*pathToFiles+"/yt-dlp/yt-dlp", arguments);
#else
        proc->start(*pathToFiles+"/youtube-dl.exe", arguments);
#endif

        this->status = videoDownloading;

        emit videoDownloadStarted(this);

        /* show output */
        connect(proc, SIGNAL(finished(int)), this, SLOT(doneDownloading()));

        return true;
    }

    return false;
}


void Video::doneDownloading(){

    if(!proc->exitStatus()){

        setAsDownloaded();

        emit videoDownloaded(this);
    }
}

void Video::stopDownload(){

    if( this->status == videoDownloading )
    {
        proc->kill();
        this->status = videoNotDownloaded;
    }
}


void Video::reset()
{
    this->status = videoNotDownloaded;

    QString listVideoDownloaded = settings->value("downloaded", "").toString();
    listVideoDownloaded.replace("/"+code, "");
    settings->setValue("downloaded", listVideoDownloaded);

    emit videoStatusChanged();
}

void Video::setAsDownloaded()
{
    if(proc != NULL)
        if(proc->state() != QProcess::NotRunning)
            stopDownload();

    this->status = videoDoneDownloaded;
    QString listVideoDownloaded = settings->value("downloaded", "").toString();
    listVideoDownloaded.append("/"+code);
    settings->setValue("downloaded", listVideoDownloaded);

    emit videoStatusChanged();
}



void Video::decodeVideoInfo(QJsonObject reply)
{
    QString videoTitle = reply.value("title").toString();
    QString releaseDate = reply.value("publishedAt").toString();

    this->title = videoTitle;
    this->releaseDate = QDateTime::fromString(releaseDate, "yyyy-MM-ddThh:mm:ss.zzzZ");
    this->haveBeenInitialised = true;
    this->isBeingInitialised = false;

    emit videoStatusChanged();
}

bool Video::lessThan(const Video *v1, const Video *v2)
{
    if(v1->getReleaseDate() > v2->getReleaseDate())
        return true;
    else
        return false;
}


VideoStatus Video::getStatus()
{
    return this->status;
}

