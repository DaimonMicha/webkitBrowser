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


class botConfig
{
public:
    bool bot;
    bool rivals;
    bool diary;
    bool heist;
    bool villa;
};

class rivalData
{
public:
    QString     r_id;
    int         r_searchTime;
    QDateTime   r_end;
};

class chAccount : public QObject
{
    Q_OBJECT

    //Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)

public:
    //chAccount(QObject *parent = 0);
    chAccount(const QString cookie, QObject *parent = 0);

    Q_INVOKABLE bool isActive(const QString option = "account") const {
        if(option == "account") return(m_config.bot);
        if(option == "rivals") return(m_config.rivals);
        if(option == "diary") return(m_config.diary);
        return(false);
    }
    Q_INVOKABLE bool isHeistActive() const { return(m_heistActive); }
    Q_INVOKABLE bool mustReload() {
        bool ret = m_mustReload;
        m_mustReload = false;
        return(ret);
    }
    Q_INVOKABLE QString cookieValue() const { return(m_cookieValue); }
    Q_INVOKABLE QString workingTitle() const;
    Q_INVOKABLE QString currentRace() const { return(m_currentRace); }
    Q_INVOKABLE QString gangster(const QString) const;
    Q_INVOKABLE QString opponent(const QString) const;
    Q_INVOKABLE QString rival(const QString) const;

    void loadFinished(QNetworkReply*);
    void loadFinished(WebPage*);

private:
    void parseCharacters(WebPage*,const QStringList paths);
    void parseChallenge(WebPage*,const QStringList paths);
    void parseCbi(WebPage*,const QStringList paths);
    void parseFights(WebPage*,const QStringList paths);
    void parseRivals(QWebFrame*,const QStringList paths);

    void setDayOfYear(int);

signals:

public slots:
    void toggle(const QString option = "account", const bool on = false);
    void click(const QString button = "nothing");
    void raceChanged(const QString);

    void opponentsListJson(const QString);
    void enemysListJson(const QString);
    void fightData(const QString);
    void getResults(const QString);
    void diaryData(const QString);
    void getBattleEventData(const QString);
    void patenvillaData(const QString);

    void rivalsData(const QString);

    void heistWork();
    void heistGetResults(const QString);

    int kwz();

private slots:
    void chooseOpponent();
    void fightRival();

private:
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
    botConfig               m_config;
    rivalData               m_rival;
};

#endif // CHACCOUNT_H
