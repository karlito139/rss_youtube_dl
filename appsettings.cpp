#include "appsettings.h"
#include "ui_appsettings.h"

AppSettings::AppSettings(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppSettings)
{
    ui->setupUi(this);
    Settings = settings;
    ui->freeDiskSpace->setValue( settings->value("disk_limit", 0).toDouble() );
}

AppSettings::~AppSettings()
{
    delete ui;
}

// Store the disk limit size in Mb
void AppSettings::on_buttonBox_accepted()
{
    // If the value changed rise a signal and store the new info
    if( ui->freeDiskSpace->text().replace(",",".").toFloat() != Settings->value("disk_limit", 0).toFloat() )
    {
        Settings->setValue("disk_limit", ui->freeDiskSpace->text().replace(",",".").toFloat());
        Settings->sync();
        emit settingsChanged();
    }
}
