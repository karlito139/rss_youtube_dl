#ifndef DISK_SPACE_H
#define DISK_SPACE_H

#include <QDialog>
#include <QObject>
#include <QSettings>

namespace Ui {
class AppSettings;
}

class AppSettings : public QDialog
{
   Q_OBJECT

public:
    explicit AppSettings(QWidget *parent = 0, QSettings *settings = 0);
    ~AppSettings();

signals:
    void disklimitChanged();

private slots:
    void on_DiskSpaceButton_accepted();

private:
  Ui::AppSettings *ui;
  QSettings *Settings;
};

#endif // DISK_SPACE_H
