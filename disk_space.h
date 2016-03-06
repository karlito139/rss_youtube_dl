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
    explicit Disk_space(QWidget *parent = 0);
    void setParent( MainWindow* parent );
    ~Disk_space();

private slots:
    void on_DiskSpaceButton_accepted();

private:
  Ui::uiDiskSpace *ui;
  MainWindow* Parent;
};

#endif // DISK_SPACE_H
