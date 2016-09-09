#ifndef INFOWORKER_H
#define INFOWORKER_H
/*
erste Änderung
*/
#include <QObject>
#include <QWebPage>

class infoWorker : public QObject
{
    Q_OBJECT
public:
    infoWorker(QObject *parent = 0);

    Q_INVOKABLE bool isActive() const { return(m_isActive); }
    int randInt(int low, int high);
    QString pageTitle();
    void setNetworkAccessManager(QNetworkAccessManager* manager)
    {
        m_workingPage->setNetworkAccessManager(manager);
    }

signals:
    void enemysList(const QString);
    void patenvilla(const QString);
    void cooldownEnd();

public slots:
    void setOn();
    void setOff();
    void fightsVip();
    void fightsStart();
    void enemysListJson(const QString);
    void patenvillaData(const QString);

private slots:
    void workFinished(bool);
    void fightsFinished();

private:
    bool                m_isActive;
    QWebPage*           m_workingPage;

    QString             m_patenvillaSecret;

};

#endif // INFOWORKER_H
