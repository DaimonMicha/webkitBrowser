#ifndef PORTMANAGER_H
#define PORTMANAGER_H

#include <QObject>

class PortManager : public QObject
{
    Q_OBJECT

public:
    explicit PortManager(QObject *parent = 0);

protected:
    void timerEvent(QTimerEvent *event);

signals:

public slots:

};

#endif // PORTMANAGER_H
