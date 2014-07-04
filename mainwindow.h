#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QStandardItem>
#include <QProcess>
#include <QSettings>

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

public slots:
  void displayingVideos();
  void outProc();
  void videoDoneDownloading(QString code);

    
private:
    Ui::MainWindow *ui;

    void installYoutubeDl();
    void downloadVideo();

    RssFeed *rssFeed;
    QList<Video *> *listVideos;

    QStringList *downloadedVideos;

    QSettings *settings;
    QProcess *proc;
    QProcess *installProc;


    QStandardItemModel *modelListVideo;


};

#endif // MAINWINDOW_H
