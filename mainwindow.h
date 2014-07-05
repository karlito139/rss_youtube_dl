#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QStandardItem>
#include <QProcess>
#include <QSettings>
#include <QFileDialog>
#include <QTimer>

#include "rssfeed.h"


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
  void outProc();
  void videoDoneDownloading(Video *vid);
  void doneInstallingYoutubeDl();

    
private slots:
  void on_browse_clicked();
  void on_downloadDestination_textChanged();
  void on_Download_clicked(bool checked);

  void recheckFeed();

private:
    Ui::MainWindow *ui;

    void installYoutubeDl();
    void downloadVideo();

    bool downloadEnable;
    bool YoutubeDlInstalled;
    RssFeed *rssFeed;
    QList<Video *> *listVideos;
    QTimer *timer;

    QProcess *proc;
    QProcess *installProc;

    QSettings *settings;


    QStandardItemModel *modelListVideo;


};

#endif // MAINWINDOW_H
