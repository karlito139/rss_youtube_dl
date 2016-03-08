#include <QStorageInfo>
#include "disk_space.h"
#include "uidiskspace.h"
#include "ui_uidiskspace.h"

Disk_space::Disk_space(QWidget *parent, QSettings* settings) :
  QDialog(parent),
  ui(new Ui::uiDiskSpace)
{
  ui->setupUi(this);
  Settings = settings;
  ui->DiskSpaceLimitBox->setMaximum(99999.99);
  ui->DiskSpaceLimitBox->setValue( settings->value("disk_limit", 0).toDouble() );
}

Disk_space::~Disk_space()
{
  delete ui;
}

// Store the disk limit size in Mb
void Disk_space::on_DiskSpaceButton_accepted()
{
    Settings->setValue("disk_limit", ui->DiskSpaceLimitBox->text().replace(",",".").toFloat());
    Settings->sync();
}
