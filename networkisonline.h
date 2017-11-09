#ifndef NETWORKISONLINE_H
#define NETWORKISONLINE_H

#include <QObject>
#include <QUrl>
#include <QString>
#include <QProcess>
#include <QTimer>

#include <QDebug>



class NetworkIsOnline : public QObject
{
    Q_OBJECT
public:
    explicit NetworkIsOnline(QUrl url, int interval = 10, QObject *parent = nullptr);
    bool isOnline();

signals:
    void onlineStateChanged(bool isOnline);
    void isNowOnline(void);
    void isNowOffline(void);

private slots:

    void doesItPing();

private:

    bool ping(QString host);

    QString distantHost;
    bool isHostOnline;
    QTimer *clocker;

};

#endif // NETWORKISONLINE_H
