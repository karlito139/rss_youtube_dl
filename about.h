#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <constants.h>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>



extern QString *pathToFiles;


namespace Ui {
class About;
}

class About : public QDialog
{
  Q_OBJECT

public:
  explicit About(QWidget *parent = 0);
  ~About();
  void checkVersion();

signals:
  void lastestVersionFetched(QString);

private slots:
  void processVersionNumber(QNetworkReply* reply);

private:
  Ui::About *ui;

  QNetworkAccessManager manager;

};

#endif // ABOUT_H
