#ifndef FIGHTWORKER_H
#define FIGHTWORKER_H

#include <QObject>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>

class fightWorker : public QObject
{
    Q_OBJECT
public:
    fightWorker(QObject *parent = 0);

    Q_INVOKABLE bool isActive() const
    {
        return(m_isActive);
    }

    Q_INVOKABLE QString opponent() const
    {
        return(m_currentOpponent);
    }

    void setNetworkAccessManager(QNetworkAccessManager* manager)
    {
        m_workingPage->setNetworkAccessManager(manager);
    }

    void setOpponent(const QString opponent)
    {
        if(m_currentOpponent != opponent) m_currentOpponent = opponent;
        if(m_currentOpponent == "") m_isActive = false;
        waitFight();
    }

    void setRival(const QString rival)
    {
        if(m_currentRival != rival) m_currentRival = rival;
    }

    int currentKWZ() const
    {
        return(m_currentKWZ);
    }

    QString pageTitle()
    {
        QWebElement title = m_workingPage->mainFrame()->findFirstElement("title");
        return(title.toPlainText().trimmed());
    }

private:
    int randInt(int low, int high)
    {
        return qrand() % ((high + 1) - low) + low;
    }

signals:
    void fightReady(int done, int max);
    void fightDataReady(const QString);
    void fightResults(const QString);
    void fightsDone();
    void fightCooldown(int seconds);

public slots:
    void setOn()
    {
        m_isActive = true;
        waitFight();
    }

    void setOff()
    {
        m_isActive = false;
    }

    void getResults(const QString result)
    {
        emit(fightResults(result));
    }

    void fightData(const QString result)
    {
        emit(fightDataReady(result));
    }

    void startFight();

private slots:
    void addJavaScriptObject()
    {
        m_workingPage->mainFrame()->addToJavaScriptWindowObject("fighter", this);
    }

    void waitFight()
    {
        if(!m_isActive) return;
        if(m_currentOpponent == "") return;
        if(m_workingPage->mainFrame()->url().path() == "/fights/waitFight") return;
        m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/fights"));
    }

    void workFinished(bool ok);

private:
    bool                m_isActive;
    QWebPage*           m_workingPage;

    QString             m_currentOpponent; // id-string
    QString             m_currentRival; // id-string
    int                 m_currentFightCounter;
    int                 m_currentKWZ;
};

#endif // FIGHTWORKER_H
