#include "uidiskspace.h"
#include "ui_uidiskspace.h"

uiDiskSpace::uiDiskSpace(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiDiskSpace)
{
    ui->setupUi(this);
}

uiDiskSpace::~uiDiskSpace()
{
    delete ui;
}
