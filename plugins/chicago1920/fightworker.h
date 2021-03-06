#ifndef FIGHTWORKER_H
#define FIGHTWORKER_H

#include <QObject>
#include <QWebPage>

class fightWorker : public QObject
{
    Q_OBJECT
public:
    fightWorker(QObject *parent = 0);

    Q_INVOKABLE bool isActive() const { return(m_isActive); }
    Q_INVOKABLE QString opponent() const { return(m_currentOpponent); }

    void setNetworkAccessManager(QNetworkAccessManager* manager)
    {
        m_workingPage->setNetworkAccessManager(manager);
    }

    void setOpponent(const QString);
    int currentKWZ() const { return(m_currentKWZ); }
    QString pageTitle();

private:
    int randInt(int low, int high);

signals:
    void fightReady(int done, int max);
    void fightDataReady(const QString);
    void fightResults(const QString);
    void fightsDone();
    void fightCooldown(int seconds);

public slots:
    void setOn();
    void setOff();
    void getResults(const QString);
    void fightData(const QString);
    void startFight();

private slots:
    void addJavaScriptObject();
    void workFinished(bool ok);
    void waitFight();

private:
    bool                m_isActive;
    QWebPage*           m_workingPage;

    QString             m_currentOpponent;
    int                 m_currentFightCounter;
    int                 m_currentKWZ;
};

#endif // FIGHTWORKER_H
