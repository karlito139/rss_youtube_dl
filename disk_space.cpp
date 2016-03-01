#include "disk_space.h"
#include "uidiskspace.h"
#include "ui_uidiskspace.h"

Disk_space::Disk_space(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::uiDiskSpace)
{
  ui->setupUi(this);
}

Disk_space::~Disk_space()
{
  delete ui;
}

void Disk_space::on_DiskSpaceButton_accepted()
{
    qDebug() << "Disk Size Limit:" + ui->DiskSpaceLimitBox->text();
}
