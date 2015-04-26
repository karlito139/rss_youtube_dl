#include "mainwindow.h"
#include "ui_mainwindow.h"





//need to be installed :
//- sudo apt-get install libgtk2.0-dev libappindicator-dev libnotify-dev


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);


  this->downloadEnable = true;
  this->starting = true;

  settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "youtube_rss_dl", "config");
  QFileInfo setting_file(settings->fileName());
  pathToFiles = new QString(setting_file.path());

  //just FYI
  //QString settingsPath = QFileInfo(settings.fileName()).absolutePath();

  //create the path to the images if it doesn't exist
  QDir resourceFolder(*pathToFiles);
  if(!resourceFolder.exists()){

      resourceFolder.mkpath(".");
  }


  QImage *img = new QImage(":/images/icon.png");
  img->save(pathToFiles->toLatin1()+"/icon.png");

  ui->downloadDestination->setText(settings->value("destination", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)+QDir::separator()).toString());

  modelListVideo = new QStandardItemModel(0, 0, this);
  modelListVideo->setColumnCount(3);
  modelListVideo->setHorizontalHeaderItem(0, new QStandardItem(QString("Title")));
  modelListVideo->setHorizontalHeaderItem(1, new QStandardItem(QString("Code")));
  modelListVideo->setHorizontalHeaderItem(2, new QStandardItem(QString("Done")));

  listVideos = NULL;
  ui->widgetListVideos->setModel(modelListVideo);

  QHeaderView *headerView = new QHeaderView(Qt::Horizontal, ui->widgetListVideos);
  ui->widgetListVideos->setHorizontalHeader(headerView);
  headerView->setSectionResizeMode(0, QHeaderView::Stretch);
  headerView->setSectionResizeMode(1, QHeaderView::Interactive);
  headerView->resizeSection(1, 150);
  headerView->resizeSection(2, 50);


  this->YoutubeDlInstalled = false;
  installYoutubeDl();


  user = settings->value("user", "");

  ui->userId->setText(user.toString());

  //Get the API key that is in the apiKey.txt file
  QFile apiKeyFile(":/apiKey.txt");
  apiKeyFile.open(QIODevice::ReadOnly);

  QTextStream apiKeyStream(&apiKeyFile);
  QString apiKey = apiKeyStream.readAll();
  apiKeyFile.close();

  apiKey.remove(QRegExp("[\\n\\t\\r]"));


  rssFeed = new RssFeed(settings, apiKey);
  connect(rssFeed, SIGNAL(doneReading()), this, SLOT(displayingVideos()));

  trayIcon = NULL;
  trayIconMenu = NULL;
  showAction = NULL;
  quitAction = NULL;
  createTrayIcon();

  timer = new QTimer();
  timer->setInterval(15*60*1000); //fetch new video every 10 minutes
  timer->start();

  connect(timer, SIGNAL(timeout()), this, SLOT(recheckFeed()));


  installProc = new QProcess();


  actionReset = NULL;
  actionDownloaded = NULL;

  ui->widgetListVideos->setContextMenuPolicy(Qt::CustomContextMenu);

  //Hide as soon as possible the app once created
  QTimer::singleShot(1, this, SLOT(close()));
}

MainWindow::~MainWindow()
{

#ifdef  Q_OS_LINUX
  //delete indicator;
  gtk_widget_destroy(menu);
  gtk_widget_destroy(showItem);
  gtk_widget_destroy(quitItem);
#endif

  delete rssFeed;
  delete timer;
  if(trayIcon)
    delete trayIcon;
  if(trayIconMenu)
    delete trayIconMenu;
  if(showAction)
    delete showAction;
  if(quitAction)
    delete quitAction;

  delete installProc;

  settings->sync();
  delete settings;

  delete modelListVideo;

  if(actionReset)
    delete actionReset;
  if(actionDownloaded)
    delete actionDownloaded;

  delete ui;
}





void MainWindow::displayingVideos(){

  int isCurrentlyDownloading = 0;
  QModelIndex currentlySelected = ui->widgetListVideos->currentIndex();

  listVideos = rssFeed->getListVideos();
  qSort(listVideos->begin(), listVideos->end(), Video::lessThan);

  for(int i=0; i<listVideos->count(); i++)
    connect(listVideos->at(i), SIGNAL(videoStatusChanged()), this, SLOT(displayingVideos()), Qt::UniqueConnection );

  modelListVideo->removeRows(0, modelListVideo->rowCount());
  modelListVideo->setRowCount(listVideos->count());

  Video *vid;
  for(int i=0; i<listVideos->count(); i++){

    vid = listVideos->at(i);

    if(vid->isCurrentlyDownloading())
      isCurrentlyDownloading++;

    modelListVideo->setItem(i, 0, new QStandardItem(vid->getTitle()));
    modelListVideo->setItem(i, 1, new QStandardItem(vid->getCode()));

    QStandardItem *item = new QStandardItem();
    QImage itemIcon;

    if(vid->haveAlreadyBeenDownloaded()) itemIcon.load(":downloaded_small");
    else if(vid->isCurrentlyDownloading()) itemIcon.load(":downloading_small");
    else itemIcon.load(":not_downloaded_small");

    itemIcon.scaled(QSize(5, 5), Qt::KeepAspectRatio);

    item->setData(QVariant(QPixmap::fromImage(itemIcon)), Qt::DecorationRole);
    modelListVideo->setItem(i, 2, item);
  }

  ui->widgetListVideos->setModel(modelListVideo);
  ui->widgetListVideos->setCurrentIndex(currentlySelected);

  if(isCurrentlyDownloading == 0)
    downloadVideo();
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



void MainWindow::installYoutubeDl()
{
  qnam.get(QNetworkRequest(QUrl("http://yt-dl.org/latest/version")));

  connect(&qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadYoutubeDlIfNecessary(QNetworkReply*)));
}


void MainWindow::downloadYoutubeDlIfNecessary(QNetworkReply* pReply)
{
  disconnect(&qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadYoutubeDlIfNecessary(QNetworkReply*)));

  QString installedVersion, currentLastVersion;
  installedVersion = settings->value("yt-dl_version", "").toString();
  currentLastVersion = QString(pReply->readAll());

  if(QString::compare(installedVersion, currentLastVersion, Qt::CaseInsensitive))
  {
#ifdef  Q_OS_LINUX
    url = "http://yt-dl.org/latest/youtube-dl.tar.gz";
#else
    url = "http://yt-dl.org/latest/youtube-dl.exe";
#endif

    qnam.get(QNetworkRequest(url));
    connect(&qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
  }
  else
  {
    doneInstallingYoutubeDl();
  }
}


void MainWindow::downloadFinished(QNetworkReply* pReply)
{
    QByteArray m_DownloadedData;
    m_DownloadedData = pReply->readAll();
    pReply->deleteLater();

#ifdef  Q_OS_LINUX
    QFile file(pathToFiles->toLatin1()+"/youtube-dl.tar.gz");
#else
    QFile file(pathToFiles->toLatin1()+"/youtube-dl.exe");
#endif

    if(file.exists())
      file.remove();
    file.open(QIODevice::WriteOnly);
    file.write(m_DownloadedData);
    file.close();

    disconnect(&qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    //Write down the version we downloaded
    qnam.get(QNetworkRequest(QUrl("http://yt-dl.org/latest/version")));
    connect(&qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(writeDownVersion(QNetworkReply*)));

#ifdef  Q_OS_LINUX
    QFile installFolder(pathToFiles->toLatin1()+"/youtube-dl");
    if(installFolder.exists())
      installFolder.remove();
    installProc->start("/bin/bash", QStringList() << "-c" << "tar -C "+pathToFiles->toLatin1()+"/ -xvf "+pathToFiles->toLatin1()+"/youtube-dl.tar.gz");
    connect(installProc, SIGNAL(finished(int)), this, SLOT(doneInstallingYoutubeDl()));
#else
    doneInstallingYoutubeDl();
#endif
}


void MainWindow::writeDownVersion(QNetworkReply* pReply)
{
  settings->setValue("yt-dl_version", QString(pReply->readAll()));
}









void MainWindow::doneInstallingYoutubeDl(){

  this->YoutubeDlInstalled = true;

#ifdef  Q_OS_LINUX
    QFile file(pathToFiles->toLatin1()+"/youtube-dl.tar.gz");
    file.remove();
#endif

  updateRSSFeed();
}

void MainWindow::videoStartDownloading(Video *){

  displayingVideos();
}

void MainWindow::videoDoneDownloading(Video *vid){

  disconnect(vid, SLOT(stopDownload()));

  QString desktop;
  bool isUnity;

  desktop = getenv("XDG_CURRENT_DESKTOP");
  isUnity = (desktop.toLower() == "unity");

  if(isUnity) //only use this in unity
  {
    system("notify-send 'Video downloaded' '"+vid->getTitle().toUtf8()+"' -i "+pathToFiles->toLatin1()+"/icon.png -t 5000");
  }
  else
  {
    trayIcon->showMessage("Video downloaded", vid->getTitle().toUtf8(), QSystemTrayIcon::Information, 5000);
  }

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

  QString desktop;
  bool isUnity;

  desktop = getenv("XDG_CURRENT_DESKTOP");
  isUnity = (desktop.toLower() == "unity");

  if(isUnity) //only use this in unity
  {
#ifdef  Q_OS_LINUX

      menu = gtk_menu_new();

      //show Item
      showItem = gtk_check_menu_item_new_with_label("Show");
      gtk_check_menu_item_set_active((GtkCheckMenuItem*)showItem, true);
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), showItem);
      g_signal_connect(showItem, "toggled", G_CALLBACK(MainWindow::showWindowGTK), qApp);

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
#endif
  }
  else //other OS's
  {

    showAction = new QAction(tr("&Show"), this);
    showAction->setCheckable(true);
    connect(showAction, SIGNAL(triggered()), this, SLOT(showWindow()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));


    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/images/icon.png"));

    trayIcon->show();
  }
}


#ifdef  Q_OS_LINUX
void MainWindow::showWindowGTK(GtkCheckMenuItem *menu, gpointer data)
{

  //http://ubuntuforums.org/showthread.php?t=2179045

  Q_UNUSED(menu);
  bool checked = gtk_check_menu_item_get_active(menu);
  QApplication *self = static_cast<QApplication *>(data);

  if(checked){

    for(int i=0; i<self->allWindows().count(); i++){

      self->allWindows().at(i)->show();
      self->allWindows().at(i)->raise();
    }
  }else{

    for(int i=0; i<self->allWindows().count(); i++){

      self->allWindows().at(i)->hide();
    }
  }
}
#endif

void MainWindow::showWindow()
{
  if(!showAction->isChecked())
    this->hide();
  else
  {
    this->show();
    this->raise();
  }
}

#ifdef  Q_OS_LINUX
void MainWindow::quitWindow(GtkMenu *menu, gpointer data){

  Q_UNUSED(menu);
  QApplication *self = static_cast<QApplication *>(data);
  self->quit();
}
#endif


void MainWindow::updateRSSFeed(){

  rssFeed->setChannelId(ui->userId->text());
}



void MainWindow::on_userId_editingFinished()
{
  if( ui->userId->text() != user)
  {
    user = ui->userId->text();

    settings->setValue("user", user.toString());
    settings->sync();

    updateRSSFeed();

    qDebug() << "finished user id";
  }
}



void MainWindow::on_actionQuite_triggered()
{
  QApplication::quit();
}


void MainWindow::closeEvent(QCloseEvent *event){

    //we hide the window
#ifdef  Q_OS_LINUX
  gtk_check_menu_item_set_active((GtkCheckMenuItem*)showItem, false);
#else
    this->hide();
    showAction->setChecked(false);
#endif

  //an ignore the close event
  event->ignore();
}




void MainWindow::on_widgetListVideos_customContextMenuRequested(const QPoint &pos)
{
  Video *vid;
  QItemSelectionModel *selections = ui->widgetListVideos->selectionModel();
  QModelIndexList selected = selections->selectedRows();
  QMenu *menu=new QMenu(this);
  bool containDownloadedVid = false;
  bool containsUndownloadedVid = false;



  if(actionReset != NULL)
    delete actionReset;
  actionReset = new QAction("Reset", this);

  if(actionDownloaded != NULL)
    delete actionDownloaded;
  actionDownloaded = new QAction("Set as downloaded", this);


  for( int i = 0 ; i < selected.size(); i++ )
  {
    vid = listVideos->at(selected[i].row());

    if(vid->haveAlreadyBeenDownloaded())
      containDownloadedVid = true;
    else
      containsUndownloadedVid = true;

    connect(actionReset, SIGNAL(triggered()), vid, SLOT(reset()));
    connect(actionDownloaded, SIGNAL(triggered()), vid, SLOT(setAsDownloaded()));
  }


  if(containDownloadedVid == true)
    menu->addAction(actionReset);

  if(containsUndownloadedVid == true)
    menu->addAction(actionDownloaded);

  menu->popup(ui->widgetListVideos->viewport()->mapToGlobal(pos));
}



void MainWindow::on_helpButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.youtube.com/account_advanced"));
}

