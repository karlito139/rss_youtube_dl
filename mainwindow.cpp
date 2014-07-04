#include "mainwindow.h"
#include "ui_mainwindow.h"



//TODO
//- check if the install of youtube dl worked well befaur download files





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);


  installYoutubeDl();

  rssFeed = new RssFeed("https://gdata.youtube.com/feeds/api/users/fczJ-auI5DysJ2n-cvh0Sg/newsubscriptionvideos");

  connect(rssFeed, SIGNAL(doneReading()), this, SLOT(displayingVideos()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::displayingVideos(){

  listVideos = rssFeed->getListVideos();

  modelListVideo = new QStandardItemModel( listVideos->count(), 2, this );

  Video *vid;
  for(int i=0; i<listVideos->count(); i++){

    vid = listVideos->at(i);

    modelListVideo->setItem(i, 0, new QStandardItem(vid->getTitle()));
    modelListVideo->setItem(i, 1, new QStandardItem(vid->getCode()));
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
  listvid->at(0)->download();

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
