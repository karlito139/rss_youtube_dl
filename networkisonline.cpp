#include "networkisonline.h"

NetworkIsOnline::NetworkIsOnline(QUrl url, int interval, QObject *parent) : QObject(parent)
{
    distantHost = url.host();   //We onli nee the host to ping not the full URL

    isHostOnline = false;   //By default we consider ourself offline

    // Configure the timer that will check the connection every interval
    clocker = new QTimer();
    clocker->setInterval(interval * 1000);
    connect(clocker, SIGNAL(timeout()), this, SLOT(doesItPing()));

    //Start the timer to count down to the next check
    clocker->start();

    //But to start as fast as possible, lets have a check now too
    if( ping(distantHost) == true )
        isHostOnline = true;    // Save the new status
    else
        isHostOnline = false;    // Save the new status
}


void NetworkIsOnline::doesItPing()
{
    if( ping(distantHost) == true )
    {
        //The distant host is rechable

        if( isHostOnline == false ) // If we used to be offline
        {
            isHostOnline = true;    // Save the new status
            emit onlineStateChanged(isHostOnline);  // Send the alarm
            emit isNowOnline();
        }
    }
    else
    {
        //The distant host is NOT rechable

        if( isHostOnline == true ) // If we used to be online
        {
            isHostOnline = false;    // Save the new status
            emit onlineStateChanged(isHostOnline);  // Send the alarm
            emit isNowOffline();
        }
    }
}


bool NetworkIsOnline::ping(QString host)
{
    QStringList parameters;
    QProcess pingProcess;

#if defined(WIN32)
    parameters << "-n" << "1";
#else
    parameters << "-c 1";
#endif

    parameters << host;

    pingProcess.start("ping", parameters);

    if (!pingProcess.waitForFinished())
        return false;

    if(pingProcess.exitCode() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool NetworkIsOnline::isOnline()
{
    return isHostOnline;
}
