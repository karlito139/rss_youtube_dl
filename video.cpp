#include "video.h"

Video::Video(QString title, QString link, QSettings *settings, QObject *parent) :
  QObject(parent)
{

  this->title = title;
  this->link = link;
  //this->code = extractCode(link);
  this->code = link;
  this->settings = settings;
  this->currentlyDownloading = false;
  this->proc = NULL;


  QString videoDownloaded = settings->value("downloaded", "").toString();
  this->alreadyDownloaded = videoDownloaded.split("/").contains(code);
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
    proc->start("/bin/bash", QStringList() << "-c" << pathToFiles->toLatin1()+"/youtube-dl/youtube-dl -f best -o '"+settings->value("destination", "").toString()+"%(title)s.%(ext)s' "+this->code);
    this->currentlyDownloading = true;

    emit videoDownloadStarted(this);

    /* show output */
    connect(proc, SIGNAL(finished(int)), this, SLOT(doneDownloading()));
    //connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(rightMessage()) );
    //connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(wrongMessage()) );
  }
}


void Video::doneDownloading(){

  if(!proc->exitStatus()){

    setAsDownloaded();

    emit videoDownloaded(this);
  }
}

void Video::stopDownload(){

  proc->kill();
  this->currentlyDownloading = false;
}


void Video::reset()
{
  this->alreadyDownloaded = false;

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

  this->alreadyDownloaded = true;
  this->currentlyDownloading = false;
  QString listVideoDownloaded = settings->value("downloaded", "").toString();
  listVideoDownloaded.append("/"+code);
  settings->setValue("downloaded", listVideoDownloaded);

  emit videoStatusChanged();
}






