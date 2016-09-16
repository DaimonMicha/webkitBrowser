#ifndef HEISTWORKER_H
#define HEISTWORKER_H

#include <QObject>
#include <QWebPage>


class heistWorker : public QObject
{
    Q_OBJECT
public:
    heistWorker(QObject *parent = 0);

signals:

public slots:
    void getResults(const QString);

private slots:
    void addJavaScriptObject();
    void workFinished(bool);

private:
    bool                m_isActive;
    QWebPage*           m_workingPage;
};

#endif // HEISTWORKER_H
