#ifndef INFOWORKER_H
#define INFOWORKER_H

#include <QObject>
#include <QTimer>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>



class gangsterData
{
public:
    QString     gd_id;
    QString     gd_coded_id;
    QString     gd_name;
    QString     gd_level;
    QString     gd_clan;
    int         gd_whisky;
    int         gd_dollar;
    int         gd_gp;
    int         gd_max_life;
};



class infoWorker : public QObject
{
    Q_OBJECT
public:
    infoWorker(QObject *parent = 0);

    QString gangster(const QString &);
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

signals:
    void diarydata(const QString);
    void enemysList(const QString);
    void patenvilla(const QString);
    void battleData(const QString);
    void cooldownEnd();

public slots:
    void setOn() {
        if(m_isActive) return;
        m_isActive = true;
        QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
    }

    void setOff() {
        m_isActive = false;
    }

    void fightsStart();
    void fightsVip();
    void placeOfHonour();
    void characters();
    void saveMax();

    // diary data
    void diaryData(const QString result) {
        emit(diarydata(result));
    }

    void enemysListJson(const QString);
    // Patenvilla Daten
    void patenvillaData(const QString result) {
        emit(patenvilla(result));
    }
    void battleEventData(const QString data) {
        emit(battleData(data));
    }

    void gangsterStatus(const QString, const QString);

private slots:
    void addJavaScriptObject() {
        m_workingPage->mainFrame()->addToJavaScriptWindowObject("worker", this);
    }

    void loadNextPage();
    void workFinished(bool);
    void fightsFinished();
    void loadStarted() {
        m_isLoading = true;
    }

private:
    bool                m_isActive;
    QWebPage*           m_workingPage;

    QString             m_patenvillaSecret;

    QStringList         m_workList;
    gangsterData        m_gangsterData;

    bool                m_isLoading;
    int                 m_minCooldown;
    int                 m_maxCooldown;

};

#endif // INFOWORKER_H
