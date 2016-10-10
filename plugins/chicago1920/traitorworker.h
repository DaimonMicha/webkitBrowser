#ifndef TRAITORWORKER_H
#define TRAITORWORKER_H

#include <QObject>
#include <QTimer>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>




class traitorWorker : public QObject
{
    Q_OBJECT
public:
    traitorWorker(QObject *parent = 0);

    Q_INVOKABLE bool isActive() const {
        return(m_isActive);
    }

    int randInt(int low, int high) {
        return(qrand() % ((high + 1) - low) + low);
    }

    QString pageTitle() {
        QWebElement title = m_workingPage->mainFrame()->findFirstElement("title");
        return(title.toPlainText().trimmed());
    }

    void setNetworkAccessManager(QNetworkAccessManager* manager) {
        m_workingPage->setNetworkAccessManager(manager);
    }

    QString traitor(const QString &);
    int currentKWZ() const {
        return(m_currentKWZ);
    }

signals:

public slots:
    void setOn() {
        if(m_isActive) return;
        m_isActive = true;
    }

    void setOff() {
        m_isActive = false;
    }

    void getResults(const QString);
    void setTraitor(const int,const int);

private slots:
    void addJavaScriptObject() {
        m_workingPage->mainFrame()->addToJavaScriptWindowObject("worker", this);
    }

    void titleChanged(const QString&);
    void loadFinished(bool);
    void startFight();

private:
    bool                m_isActive;
    bool                m_timerActive;
    QWebPage*           m_workingPage;

    int                 m_minCooldown;
    int                 m_maxCooldown;
    int                 m_currentRow;
    int                 m_currentColumn;
    int                 m_currentKWZ;
};

#endif // TRAITORWORKER_H
