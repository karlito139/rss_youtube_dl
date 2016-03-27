#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::About)
{
  ui->setupUi(this);

  ui->newVersionLabel->hide();

  ui->version->setText(CURRENT_VERSION);

  checkVersion();
}

About::~About()
{
  delete ui;
}

void About::checkVersion()
{
  QString url;
  url = "http://localtube.org/latestVersion.json";

  QNetworkRequest request(url);
  manager.get(request);
  connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processVersionNumber(QNetworkReply*)), Qt::UniqueConnection);
}


void About::processVersionNumber(QNetworkReply* reply)
{
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  if (statusCode >= 200 && statusCode < 300) {
      QString data = (QString)reply->readAll();

      QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
      QJsonObject jsonResponseObj = jsonResponse.object();

      QString latestVersion = jsonResponseObj.value("latestVersion").toString();

      //qDebug() << "My version is : " << CURRENT_VERSION << "; the latest one is : " << latestVersion;

      emit lastestVersionFetched(latestVersion);

      if( latestVersion.compare(CURRENT_VERSION) > 0)
      {
        ui->newVersionLabel->show();
      }
  }
}

