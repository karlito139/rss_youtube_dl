#ifndef UIDISKSPACE_H
#define UIDISKSPACE_H

#include <QDialog>
#include "uidiskspace.h"
#include "ui_uidiskspace.h"

namespace Ui {
class uiDiskSpace;
}

class uiDiskSpace : public QDialog
{
    Q_OBJECT

public:
    explicit uiDiskSpace(QWidget *parent = 0);
    ~uiDiskSpace();

private:
    Ui::uiDiskSpace *ui;
};

#endif // UIDISKSPACE_H
