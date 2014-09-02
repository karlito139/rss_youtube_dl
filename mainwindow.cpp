#include "mainwindow.h"
#include "ui_mainwindow.h"




//need to be installed :
//- sudo apt-get install libgtk2.0-dev libappindicator-dev libnotify-dev




//TODO
//* add the user rss feed to the config file
//* debug all warnings
//* the file/quit doesn't destroy the app, only the window
//* ajouter une config complète (setting du l'id de l'utilisateur)
//* (durty hak)debug de pourquoi la fenetre ne se cache pas
//* quand on quite (croix) on cache en fait
//* ajouter un menu fichier/quitter pour vraiement quitter
//* tester si il y a déjà des fichiers dl pour yt dl (éviter les entassement de fichiers)
//* don't reset the config of the list (sizes) when we update it.
//* use the home folder to store the config
//* use the resource file for the GTK icon
//* the download folder must have a / at the end
//* (in the downloads folder of home) what append when we don't have setted any download folder

//- move that todo list to the issues of github
//- doxygen/QT documentation
//- when stating, start hidden
//- destroy everything we created when quitting the app
//- ajouter la date et heure du dernier check des videos
//- put icon for the downloaded and not yet downloaded
//- ajouter une fenetre de config (update rate of the videos, definition of the videos downloaded)
//- dl youtube-dl in the config folder



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);


  this->downloadEnable = true;

  this->starting = true;

  settings = new QSettings("youtube_rss_dl", "config");

  QFileInfo setting_file(settings->fileName());
  pathToFiles = new QString(setting_file.path());


  //wreate the path to the images if it doesn't exist
  QDir resourceFolder(*pathToFiles);
  if(!resourceFolder.exists()){

      resourceFolder.mkpath(".");
  }


  QImage *img = new QImage(":/images/icon.png");
  img->save(pathToFiles->toLatin1()+"/icon.png");


  ui->downloadDestination->setText(settings->value("destination", "~/Downloads/").toString());

  modelListVideo = new QStandardItemModel(0, 0, this);
  modelListVideo->setColumnCount(3);
  modelListVideo->setHorizontalHeaderItem(0, new QStandardItem(QString("Title")));
  modelListVideo->setHorizontalHeaderItem(1, new QStandardItem(QString("Code")));
  modelListVideo->setHorizontalHeaderItem(2, new QStandardItem(QString("Done")));

  ui->widgetListVideos->setModel(modelListVideo);

  QHeaderView *headerView = new QHeaderView(Qt::Horizontal, ui->widgetListVideos);
  ui->widgetListVideos->setHorizontalHeader(headerView);
  headerView->setSectionResizeMode(0, QHeaderView::Stretch);
  headerView->setSectionResizeMode(1, QHeaderView::Interactive);
  headerView->resizeSection(1, 150);
  headerView->resizeSection(2, 50);



  this->currentlyDownloading = false;
  this->YoutubeDlInstalled = false;
  installYoutubeDl();


  user = settings->value("user", "");

  ui->userId->setText(user.toString());


  rssFeed = new RssFeed(settings);
  connect(rssFeed, SIGNAL(doneReading()), this, SLOT(displayingVideos()));
  updateRSSFeed();


  createTrayIcon();

  timer = new QTimer();
  timer->setInterval(10*60*1000); //fetch new video every 10 minutes
  timer->start();

  connect(timer, SIGNAL(timeout()), this, SLOT(recheckFeed()));
}


MainWindow::~MainWindow()
{
  delete installProc;
  installProc = new QProcess();
  installProc->start("/bin/bash", QStringList() << "-c" << "rm -r youtube-dl");

  settings->sync();

  delete ui;
}


void MainWindow::displayingVideos(){

  listVideos = rssFeed->getListVideos();

  modelListVideo->removeRows(0, modelListVideo->rowCount());
  modelListVideo->setRowCount(listVideos->count());

  Video *vid;
  for(int i=0; i<listVideos->count(); i++){

    vid = listVideos->at(i);

    modelListVideo->setItem(i, 0, new QStandardItem(vid->getTitle()));
    modelListVideo->setItem(i, 1, new QStandardItem(vid->getCode()));
    if(vid->haveAlreadyBeenDownloaded()) modelListVideo->setItem(i, 2, new QStandardItem("yes"));
    else if(vid->isCurrentlyDownloading()) modelListVideo->setItem(i, 2, new QStandardItem("..."));
    else modelListVideo->setItem(i, 2, new QStandardItem("no"));
  }

  ui->widgetListVideos->setModel(modelListVideo);

  if(!currentlyDownloading) downloadVideo();

}



void MainWindow::downloadVideo(){

  if( (this->downloadEnable) && (this->YoutubeDlInstalled) ){

    QList<Video *> *listvid = rssFeed->getListVideos();
    for(int i=0; i<listvid->count(); i++){

      if(!listvid->at(i)->haveAlreadyBeenDownloaded()){

        connect(listvid->at(i), SIGNAL(videoDownloaded(Video *)), this, SLOT(videoDoneDownloading(Video *)));
        connect(listvid->at(i), SIGNAL(videoDownloadStarted(Video*)), this, SLOT(videoStartDownloading(Video*)));
        connect(this, SIGNAL(stopDownloading()), listvid->at(i), SLOT(stopDownload()));

        listvid->at(i)->download();

        break;
      }
    }
  }
}



void MainWindow::installYoutubeDl(){

  installProc = new QProcess();
  installProc->start("/bin/bash", QStringList() << "-c" << "wget http://yt-dl.org/latest/youtube-dl.tar.gz -O youtube-dl.tar.gz && tar -xvf youtube-dl.tar.gz && rm youtube-dl.tar.gz");

  connect(installProc, SIGNAL(finished(int)), this, SLOT(doneInstallingYoutubeDl()));

}

void MainWindow::doneInstallingYoutubeDl(){

  this->YoutubeDlInstalled = true;
  displayingVideos();
}

void MainWindow::videoStartDownloading(Video *){

  this->currentlyDownloading = true;
  displayingVideos();
}

void MainWindow::videoDoneDownloading(Video *vid){

  disconnect(vid, SLOT(stopDownload()));
  this->currentlyDownloading = false;
  system("notify-send 'Video downloaded' '"+vid->getTitle().toUtf8()+"' -i "+pathToFiles->toLatin1()+"/icon.png -t 5000");
  displayingVideos();
}

void MainWindow::on_browse_clicked()
{
  QString path = QFileDialog::getExistingDirectory (this, tr("Directory"));
  if ( path.isNull() == false )
  {
      ui->downloadDestination->setText(path+"/");
  }
}

void MainWindow::on_downloadDestination_textChanged()
{
  settings->setValue("destination", ui->downloadDestination->text());
}


void MainWindow::recheckFeed(){

  rssFeed->fetch();
}


void MainWindow::createTrayIcon(){

    /*trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(new QAction(tr("Mi&nimize"), this));*/

    /*trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);*/

    /*trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/images/icon.png"));

    trayIcon->show();*/






  QString desktop;
  bool isUnity;

  desktop = getenv("XDG_CURRENT_DESKTOP");
  isUnity = (desktop.toLower() == "unity");

  if(isUnity) //only use this in unity
  {
      menu = gtk_menu_new();

      //show Item
      showItem = gtk_check_menu_item_new_with_label("Show");
      gtk_check_menu_item_set_active((GtkCheckMenuItem*)showItem, true);
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), showItem);
      g_signal_connect(showItem, "toggled", G_CALLBACK(MainWindow::showWindow), qApp);

      //g_signal_connect()
      gtk_widget_show(showItem);

      //quit item
      quitItem = gtk_menu_item_new_with_label("Quit");
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), quitItem);
      g_signal_connect(quitItem, "activate", G_CALLBACK(MainWindow::quitWindow), qApp);
      gtk_widget_show(quitItem);

      indicator = app_indicator_new("example", "indicator-messages", APP_INDICATOR_CATEGORY_OTHER);

      app_indicator_set_icon_theme_path(indicator, pathToFiles->toLatin1());
      app_indicator_set_icon_full(indicator, "icon", "description");

      app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
      app_indicator_set_menu(indicator, GTK_MENU(menu));
  }
  else //other OS's
  {

  }

}



void MainWindow::showWindow(GtkCheckMenuItem *menu, gpointer data){

  //http://ubuntuforums.org/showthread.php?t=2179045

  Q_UNUSED(menu);
  bool checked = gtk_check_menu_item_get_active(menu);
  QApplication *self = static_cast<QApplication *>(data);

  if(checked){

    for(int i=0; i<self->allWindows().count(); i++){

      self->allWindows().at(i)->show();
    }
  }else{

    for(int i=0; i<self->allWindows().count(); i++){

      self->allWindows().at(i)->hide();
    }
  }
}


void MainWindow::quitWindow(GtkMenu *menu, gpointer data){

  Q_UNUSED(menu);
  QApplication *self = static_cast<QApplication *>(data);
  self->quit();
}



void MainWindow::updateRSSFeed(){

  rssFeed->setURL("https://gdata.youtube.com/feeds/api/users/"+user.toString()+"/newsubscriptionvideos");
}



void MainWindow::on_userId_editingFinished()
{
  user = ui->userId->text();

  settings->setValue("user", user.toString());
  settings->sync();

  updateRSSFeed();
}



void MainWindow::on_actionQuite_triggered()
{
  QApplication::quit();
}


void MainWindow::closeEvent(QCloseEvent *event){

  //we hide the window
  gtk_check_menu_item_set_active((GtkCheckMenuItem*)showItem, false);

  //an ignore the close event
  event->ignore();
}



