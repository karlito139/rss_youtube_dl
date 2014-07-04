#include "video.h"

Video::Video(QString title, QString link, QObject *parent) :
  QObject(parent)
{

  this->title = title;
  this->link = link;
  this->code = extractCode(link);

  this->alreadyDownloaded = false;



}


QString Video::extractCode(QString link){

  QRegExp regExp("videos/.*");

  regExp.indexIn(link, 0);

  return regExp.cap(0).mid(7,-1);
}


void Video::download(){


  /* create QProcess object */
  proc= new QProcess();
  proc->start("/bin/bash", QStringList() << "-c" << "youtube-dl/youtube-dl -f best https://www.youtube.com/watch?v="+this->code);

  /* show output */
  connect(proc, SIGNAL(finished(int)), this, SLOT(doneDownloading()));
  //connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(rightMessage()) );
  //connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(wrongMessage()) );

}


void Video::doneDownloading(){

  //qDebug() << proc->readAllStandardOutput();
  this->alreadyDownloaded = true;

}
