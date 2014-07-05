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

#include "rssfeed.h"





#define THEME_PATH "/home/karlito/creation/rss_youtube_dl/images/"


#undef signals
extern "C"
{
    #include <libappindicator/app-indicator.h>
    #include <libnotify/notify.h>
    #include <gtk/gtk.h>

void onShow(GtkCheckMenuItem *, gpointer);
void onQuit(GtkMenu *, gpointer);
}
#define signals public



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
  void videoStartDownloading(Video *vid);
  void doneInstallingYoutubeDl();
  static void showWindow(GtkMenu *menu, gpointer data); //static needed for GTK
  static void quitWindow(GtkMenu *menu, gpointer data);


    
private slots:
  void on_browse_clicked();
  void on_downloadDestination_textChanged();
  void on_Download_clicked(bool checked);

  void recheckFeed();


private:
    Ui::MainWindow *ui;

    void installYoutubeDl();
    void downloadVideo();
    void createTrayIcon();




    bool downloadEnable;
    bool YoutubeDlInstalled;
    bool currentlyDownloading;
    RssFeed *rssFeed;
    QList<Video *> *listVideos;
    QTimer *timer;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;



    QProcess *proc;
    QProcess *installProc;

    QSettings *settings;


    QStandardItemModel *modelListVideo;


};

#endif // MAINWINDOW_H
