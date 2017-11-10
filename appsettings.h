#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class AppSettings;
}

class AppSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AppSettings(QSettings *settings, QWidget *parent = 0);
    ~AppSettings();

signals:
    void settingsChanged();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AppSettings *ui;
    QSettings *Settings;

};

#endif // APPSETTINGS_H
