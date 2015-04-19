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

#include "rssfeed.h"

extern QString *pathToFiles;


#ifdef  Q_OS_LINUX
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
#endif


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

#ifdef  Q_OS_LINUX
  static void showWindowGTK(GtkCheckMenuItem *menu, gpointer data); //static needed for GTK
  static void quitWindow(GtkMenu *menu, gpointer data);
#endif

  void showWindow();

    
private slots:
  void on_browse_clicked();
  void on_downloadDestination_textChanged();
  void updateRSSFeed();

  void recheckFeed();
  void on_userId_editingFinished();
  void on_actionQuite_triggered();

  void downloadFinished(QNetworkReply* pReply);
  void writeDownVersion(QNetworkReply* pReply);
  void downloadYoutubeDlIfNecessary(QNetworkReply* pReply);
  void on_widgetListVideos_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;

    void installYoutubeDl();
    void downloadVideo();
    void createTrayIcon();

#ifdef  Q_OS_LINUX
    AppIndicator *indicator;
    GtkWidget *menu;
    GtkWidget *showItem;
    GtkWidget *quitItem;
#endif

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

    QNetworkAccessManager qnam;
    QUrl url;

    QPixmap buttonImage;
    QGraphicsScene scene;
    QGraphicsPixmapItem item;

    QAction *actionReset;
    QAction *actionDownloaded;


protected:
     void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
