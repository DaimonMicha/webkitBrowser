#ifndef INFOWORKER_H
#define INFOWORKER_H
/*
erste Änderung
*/
#include <QObject>
#include <QWebPage>



class gangsterData
{
public:
    QString     gd_id;
    QString     gd_coded_id;
    QString     gd_name;
    QString     gd_level;
    QString     gd_clan;
};



class infoWorker : public QObject
{
    Q_OBJECT
public:
    infoWorker(QObject *parent = 0);

    QString gangster(const QString &);
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
    void fightsStart();
    void fightsVip();
    void placeOfHonour();
    void characters();

    void enemysListJson(const QString);
    void patenvillaData(const QString);

private slots:
    void addJavaScriptObject();
    void titleChanged(const QString &);
    void loadNextPage();
    void workFinished(bool);
    void fightsFinished();
    void loadStarted();

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
