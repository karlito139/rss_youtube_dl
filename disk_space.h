#ifndef DISK_SPACE_H
#define DISK_SPACE_H

#include <QDialog>
#include <QObject>
#include <mainwindow.h>
#include "uidiskspace.h"
#include "ui_uidiskspace.h"

namespace Ui {
class Disk_space;
}

class Disk_space : public QDialog
{
   Q_OBJECT

public:
    explicit Disk_space(QWidget *parent = 0, QSettings* settings = 0);
    ~Disk_space();

signals:
    void disklimitChanged();

private slots:
    void on_DiskSpaceButton_accepted();

private:
  Ui::uiDiskSpace *ui;
  QSettings* Settings;
};

#endif // DISK_SPACE_H
