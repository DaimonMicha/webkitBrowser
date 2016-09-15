#ifndef CHACCOUNT_H
#define CHACCOUNT_H

#include <QObject>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QWebPage>
#include "webview.h"

#include "chplayertable.h"
#include "infoworker.h"
#include "fightworker.h"





class chAccount : public QObject
{
    Q_OBJECT

    //Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)

public:
    //chAccount(QObject *parent = 0);
    chAccount(const QString cookie, QObject *parent = 0);

    Q_INVOKABLE bool isActive() const { return(m_isActive); }
    Q_INVOKABLE bool isHeistActive() const { return(m_heistActive); }
    Q_INVOKABLE bool mustReload() {
        bool ret = m_mustReload;
        m_mustReload = false;
        return(ret);
    }
    Q_INVOKABLE QString cookieValue() const { return(m_cookieValue); }
    Q_INVOKABLE QString workingTitle() const;
    Q_INVOKABLE QString opponent(const QString) const;

    void loadFinished(QNetworkReply*);
    void loadFinished(WebPage*);

private:
    void parseCharacters(WebPage*,const QStringList paths);
    void parseChallenge(WebPage*,const QStringList paths);
    void parseCbi(WebPage*,const QStringList paths);
    void parseFights(WebPage*,const QStringList paths);

    void injectHtml(QWebFrame*);
    int readDataFile(const QString file, QString& data);

signals:

public slots:
    void toggle(const QString option = "account", const bool on = false);
    void click(const QString button = "nothing");
    void raceChanged(const QString);

    void opponentsListJson(const QString);
    void fightData(const QString);
    void getResults(const QString);
    void enemysListJson(const QString);
    void diaryData(const QString);
    void getBattleEventData(const QString);
    void patenvillaData(const QString);

    void heistToggle(const bool);
    void heistWork();
    void heistGetResults(const QString);

    int kwz();

private slots:
    void workFinished(bool ok);
    void chooseOpponent();

private:
    bool                    m_isActive;
    bool                    m_heistActive;
    bool                    m_mustReload;
    QString                 m_cookieValue;
    QString                 m_nameValue;
    QString                 m_levelValue;

    QString                 m_currentOpponent; //fragwürdig, bekommen wir vom fightWorker...
    QString                 m_currentRace;
    qint32                  m_currentDay;

    QWebPage*               m_workingPage;
    chPlayerTable*          m_gangstersTable;

    infoWorker*             m_infoWorker;
    fightWorker*            m_fightWorker;

    QMap<QString, QString>  m_cityMap;
};

#endif // CHACCOUNT_H
