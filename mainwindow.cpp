#include "mainwindow.h"
#include "ui_mainwindow.h"



//TODO
//- check if the install of youtube dl worked well befaur download files




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  downloadedVideos = new QStringList();

  settings = new QSettings(QString("configs/config.ini"), QSettings::IniFormat);
  QString videoDownloaded = settings->value("downloaded", "").toString();
  *downloadedVideos = videoDownloaded.split("/");

  installYoutubeDl();

  rssFeed = new RssFeed("https://gdata.youtube.com/feeds/api/users/fczJ-auI5DysJ2n-cvh0Sg/newsubscriptionvideos", settings, downloadedVideos);

  connect(rssFeed, SIGNAL(doneReading()), this, SLOT(displayingVideos()));

}

MainWindow::~MainWindow()
{
  settings->setValue("downloaded", downloadedVideos->join("/"));
  settings->setValue("destination", "/home/karlito/Downloads/a_voir/");
  settings->sync();

  delete ui;
}


void MainWindow::displayingVideos(){

  listVideos = rssFeed->getListVideos();

  modelListVideo = new QStandardItemModel( listVideos->count(), 3, this );

  Video *vid;
  for(int i=0; i<listVideos->count(); i++){

    vid = listVideos->at(i);

    modelListVideo->setItem(i, 0, new QStandardItem(vid->getTitle()));
    modelListVideo->setItem(i, 1, new QStandardItem(vid->getCode()));
    if(vid->haveAlreadyBeenDownloaded()) modelListVideo->setItem(i, 2, new QStandardItem("yes"));
    else modelListVideo->setItem(i, 2, new QStandardItem("no"));
  }

  ui->widgetListVideos->setModel(modelListVideo);

  downloadVideo();
}



void MainWindow::downloadVideo(){

  /* create QProcess object */
  //proc= new QProcess();
  //proc->start("/bin/bash", QStringList() << "-c" << "youtube-dl/youtube-dl -f best ");

  /* show output */
  //connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(outProc()) );
  //connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(wrongMessage()) );

  QList<Video *> *listvid = rssFeed->getListVideos();
  //listvid->at(1)->download();
  for(int i=0; i<listvid->count(); i++){

      if(!listvid->at(i)->haveAlreadyBeenDownloaded()){

          listvid->at(i)->download();
          connect(listvid->at(i), SIGNAL(videoDownloaded(QString)), this, SLOT(videoDoneDownloading(QString)));
          break;
      }
  }


}


void MainWindow::outProc(){


  QByteArray strdata = proc->readAllStandardOutput();
  /*ui->txtReport->setTextColor(Qt::black);
  ui->txtReport->append(strdata);*/

  qDebug() << strdata;
}



void MainWindow::installYoutubeDl(){

  installProc = new QProcess();
  installProc->start("/bin/bash", QStringList() << "-c" << "wget http://yt-dl.org/latest/youtube-dl.tar.gz && tar -xvf youtube-dl.tar.gz && rm youtube-dl.tar.gz");
}


void MainWindow::videoDoneDownloading(QString code){

  downloadedVideos->append(code);
  displayingVideos();
}
