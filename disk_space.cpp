#include <QStorageInfo>
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

void Disk_space::setParent( MainWindow* parent )
{
    Parent = parent;
}

void Disk_space::on_DiskSpaceButton_accepted()
{
    qDebug() << "Disk Size Limit:" + ui->DiskSpaceLimitBox->text();
    QStorageInfo storage = QStorageInfo::root();

    qDebug() << storage.rootPath();
    if (storage.isReadOnly())
        qDebug() << "isReadOnly:" << storage.isReadOnly();

    qDebug() << "name:" << storage.name();
    qDebug() << "fileSystemType:" << storage.fileSystemType();
    qDebug() << "size:" << storage.bytesTotal()/1000/1000 << "MB";
    qDebug() << "availableSize:" << storage.bytesAvailable()/1000/1000 << "MB";

    Parent->storeDiskLimit( ui->DiskSpaceLimitBox->text().toFloat() );

    //MainWindow parent = qobject_cast<MainWindow>(this->parent());

    //  if (parent.isWindow() == 0) { return; } // or some other error handling

   // parent->storeDiskLimit( /*ui->DiskSpaceLimitBox->text().toFloat()*/ );
}
