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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QStandardItem>
#include <QProcess>
#include <QSettings>
#include <QFileDialog>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QWindow>
#include <QHeaderView>
#include <QCloseEvent>
#include <QFileIconProvider>
#include <QMessageBox>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <QAction>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QItemSelectionModel>
#include <QDesktopServices>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QLabel>
#include <QTime>
#include <about.h>

#include "rssfeed.h"




#define CURRENT_VERSION    "v1.1"




extern QString *pathToFiles;



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

signals:
  void stopDownloading();

public slots:
  void displayingVideos();
  void videoDoneDownloading(Video *vid);
  void videoStartDownloading(Video *);
  void doneInstallingYoutubeDl();
  void showWindow();


private slots:
  void on_browse_clicked();
  void on_downloadDestination_textChanged();
  void updateRSSFeed();

  void on_actionQuite_triggered();

  void downloadFinished(QNetworkReply* pReply);
  void writeDownVersion(QNetworkReply* pReply);
  void downloadYoutubeDlIfNecessary(QNetworkReply* pReply);
  void on_widgetListVideos_customContextMenuRequested(const QPoint &pos);

  void on_helpButton_clicked();
  void on_loginButton_clicked();

  void updateUI();
  void decodeAuthToken(QNetworkReply* reply);
  void on_authCode_textChanged();

  void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;

    void installYoutubeDl();
    void downloadVideo();
    void createTrayIcon();
    bool downloadEnable;
    bool YoutubeDlInstalled;
    bool starting;
    RssFeed *rssFeed;
    QList<Video *> *listVideos;
    QTimer *timer;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QProcess *installProc;
    QSettings *settings;
    QVariant user;
    QStandardItemModel *modelListVideo;

    QAction *showAction;
    QAction *quitAction;
    QLabel statusBarText;

    QNetworkAccessManager qnam;
    QUrl url;

    QPixmap buttonImage;
    QGraphicsScene scene;
    QGraphicsPixmapItem item;

    QAction *actionReset;
    QAction *actionDownloaded;

    QString clientId;
    QString clientSecret;


    QNetworkAccessManager networkManager;

protected:
     void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
