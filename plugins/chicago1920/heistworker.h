#ifndef HEISTWORKER_H
#define HEISTWORKER_H

#include <QObject>

class heistWorker : public QObject
{
    Q_OBJECT
public:
    explicit heistWorker(QObject *parent = 0);

signals:

public slots:

private slots:
    void addJavaScriptObject();
};

#endif // HEISTWORKER_H
