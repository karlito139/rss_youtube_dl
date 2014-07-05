#include "mainwindow.h"
#include "ui_mainwindow.h"



//TODO
//- put icon for the downloaded and not yet downloaded
//- ajouter un status en cours de dl dans le tableau
//- ajouter l'option de minimisation de l'application : https://qt-project.org/doc/qt-4.8/desktop-systray.html



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  this->downloadEnable = true;

  settings = new QSettings(QString("configs/config.ini"), QSettings::IniFormat);
  ui->downloadDestination->setText(settings->value("destination", "").toString());

  modelListVideo = new QStandardItemModel(0, 0, this);

  this->YoutubeDlInstalled = false;
  installYoutubeDl();

  rssFeed = new RssFeed("https://gdata.youtube.com/feeds/api/users/fczJ-auI5DysJ2n-cvh0Sg/newsubscriptionvideos", settings);

  connect(rssFeed, SIGNAL(doneReading()), this, SLOT(displayingVideos()));


  timer = new QTimer();
  timer->setInterval(1*60*1000);
  timer->start();

  connect(timer, SIGNAL(timeout()), this, SLOT(recheckFeed()));
}

MainWindow::~MainWindow()
{
  delete installProc;
  installProc = new QProcess();
  installProc->start("/bin/bash", QStringList() << "-c" << "rm -r youtube-dl");


  settings->setValue("destination", "/home/karlito/Downloads/a_voir/");
  settings->sync();

  delete ui;
}


void MainWindow::displayingVideos(){

  listVideos = rssFeed->getListVideos();

  modelListVideo->clear();
  modelListVideo->setColumnCount(3);
  modelListVideo->setRowCount(listVideos->count());

  modelListVideo->setHorizontalHeaderItem(0, new QStandardItem(QString("Title")));
  modelListVideo->setHorizontalHeaderItem(1, new QStandardItem(QString("Code")));
  modelListVideo->setHorizontalHeaderItem(2, new QStandardItem(QString("Downloaded")));

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

  if( (this->downloadEnable) && (this->YoutubeDlInstalled) ){

    QList<Video *> *listvid = rssFeed->getListVideos();
    for(int i=0; i<listvid->count(); i++){

      if(!listvid->at(i)->haveAlreadyBeenDownloaded()){

        listvid->at(i)->download();
        connect(listvid->at(i), SIGNAL(videoDownloaded(Video *)), this, SLOT(videoDoneDownloading(Video *)));
        connect(this, SIGNAL(stopDownloading()), listvid->at(i), SLOT(stopDownload()));
        break;
      }
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

  connect(installProc, SIGNAL(finished(int)), this, SLOT(doneInstallingYoutubeDl()));

}

void MainWindow::doneInstallingYoutubeDl(){

  this->YoutubeDlInstalled = true;
}

void MainWindow::videoDoneDownloading(Video *vid){

  disconnect(vid, SLOT(stopDownload()));
  //TODO : add icon to notifications :
  // -i /usr/share/pixmaps/idle.xpm
  system("notify-send 'Video downloaded' '"+vid->getTitle().toUtf8()+"' '-t' 5000");
  displayingVideos();
}

void MainWindow::on_browse_clicked()
{
  QString path = QFileDialog::getExistingDirectory (this, tr("Directory"));
  if ( path.isNull() == false )
  {
      ui->downloadDestination->setText(path);
  }
}

void MainWindow::on_downloadDestination_textChanged()
{
  settings->setValue("destination", ui->downloadDestination->text());
}

void MainWindow::on_Download_clicked(bool checked)
{
  this->downloadEnable = checked;
  if(!this->downloadEnable) emit stopDownloading();
  else displayingVideos();
}


void MainWindow::recheckFeed(){

  rssFeed->fetch();
}
