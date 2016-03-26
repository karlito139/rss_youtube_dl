#include <QStorageInfo>
#include "appSettings.h"

AppSettings::AppSettings(QWidget *parent, QSettings *settings) :
  QDialog(parent),
  ui(new Ui::AppSettings)
{
  ui->setupUi(this);
  Settings = settings;
  ui->DiskSpaceLimitBox->setMaximum(99999.99);
  ui->DiskSpaceLimitBox->setValue( settings->value("disk_limit", 0).toDouble() );
}

AppSettings::~AppSettings()
{
  delete ui;
}

// Store the disk limit size in Mb
void AppSettings::on_DiskSpaceButton_accepted()
{
    // If the value changed rise a signal and store the new info
    if( ui->DiskSpaceLimitBox->text().replace(",",".").toFloat() != Settings->value("disk_limit", 0).toFloat() )
    {
        Settings->setValue("disk_limit", ui->DiskSpaceLimitBox->text().replace(",",".").toFloat());
        Settings->sync();
        emit disklimitChanged();
    }
}
