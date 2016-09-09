#include "portmanager.h"
#include <QFile>
#include <QDebug>

PortManager::PortManager(QObject *parent) :
    QObject(parent)
{
    startTimer(250);
}

void PortManager::timerEvent(QTimerEvent *)
{
    QFile file("/proc/net/tcp");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "failed to open /proc/net/tcp";
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    //int max,ist,cache,swap;
    int lines = 0;
    while(!line.isNull()) {
        QStringList parts = line.split(" ", QString::SkipEmptyParts);
/*
        if(parts.at(0) == "MemTotal:") {
            max = parts.at(1).toInt();
            if(max != maximum()) setMaximum(max);
        }
        if(parts.at(0) == "MemFree:") {
            ist = parts.at(1).toInt();
        }
        if(parts.at(0) == "Cached:") {
            cache = parts.at(1).toInt();
        }
        if(parts.at(0) == "SwapCached:") {
            swap = parts.at(1).toInt();
        }
*/
        line = in.readLine();
        lines++;
    }
    file.close();
    //qDebug() << lines;
}
