#include "video.h"

Video::Video(QString title, QString link, QSettings *settings, QStringList *downloadedVideos, QObject *parent) :
  QObject(parent)
{

  this->title = title;
  this->link = link;
  this->code = extractCode(link);
  this->settings = settings;

  this->alreadyDownloaded = downloadedVideos->contains(code);
}


QString Video::extractCode(QString link){

  QRegExp regExp("videos/.*");

  regExp.indexIn(link, 0);

  return regExp.cap(0).mid(7,-1);
}


void Video::download(){

  if(!alreadyDownloaded){
    /* create QProcess object */
    proc= new QProcess();
    proc->start("/bin/bash", QStringList() << "-c" << "youtube-dl/youtube-dl -f best -o '"+settings->value("destination", "").toString()+"%(title)s.%(ext)s' "+this->code);

    /* show output */
    connect(proc, SIGNAL(finished(int)), this, SLOT(doneDownloading()));
    //connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(rightMessage()) );
    //connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(wrongMessage()) );
  }
}


void Video::doneDownloading(){

  //qDebug() << proc->readAllStandardOutput();
  this->alreadyDownloaded = true;

  emit videoDownloaded(this->code);
}
