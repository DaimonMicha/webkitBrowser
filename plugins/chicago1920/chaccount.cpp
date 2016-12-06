#include "chaccount.h"

#include <QCryptographicHash>
#include <QDesktopServices>

#include <QWebFrame>
#include <QWebElement>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QDate>
#include <QDateTime>
#include <QFile>

#include <QDebug>



chAccount::chAccount(const QString cookie, QObject *parent) :
    QObject(parent),
    m_heistActive(false),
    m_mustReload(false),
    m_cookieValue(cookie),
    m_currentDay(0),
    m_gangstersTable(new chPlayerTable),
    m_dataParser(new dataParser(m_gangstersTable))
{
    //m_cookieValue = cookie;
    m_config.bot = false;
    m_config.opponents = false;
    m_config.rivals = false;
    m_config.diary = false;
    m_config.traitor = false;
    m_config.autosaver = false;
    m_config.maxMoney = 2486;

    m_rival.r_searchTime = 0;
    m_rival.r_end = QDateTime::currentDateTime();
    m_rival.r_id = "";

    m_infoWorker = NULL;
    m_fightWorker = NULL;
    m_traitorWorker = NULL;
    m_currentRace = "0";

    p_lastReply = NULL;
    p_replyData = "";

}

QString chAccount::gangster(const QString field) const
{
    QString ret;

    if(!m_infoWorker) return(ret);

    if(field == "name") {
        ret = m_infoWorker->gangster("name");
    }

    return(ret);
}

QString chAccount::opponent(const QString field) const
{
    QString pid = m_fightWorker->opponent();
    QString ret = m_gangstersTable->getPlayerData(pid, field);

    QJsonDocument json = m_dataParser->lastResult();

    if(field == QString("lastLoot") && !json.isEmpty()) {
        QJsonObject results(json.object().value("results").toObject());
        QJsonObject player1(results.value("p1").toObject());
        ret = QString("%1").arg(player1.value("gold").toDouble());
    }
    return(ret);
}

QString chAccount::rival(const QString field) const
{
    QString ret = "";

    if(field == "allTime") {
        ret = QString("%1").arg(m_rival.r_searchTime);

    } else if(field == "currentTime") {
        QDateTime now = QDateTime::currentDateTime();
        if(now < m_rival.r_end) {
            ret = QString("%1").arg(m_rival.r_searchTime - now.secsTo(m_rival.r_end));
        //} else {
            //ret = QString("%1").arg(m_rival.r_searchTime);
        }

    } else if(field == "timeString") {
        QDateTime now = QDateTime::currentDateTime();
        if(now < m_rival.r_end) {
            QTime mid = QTime(0,0,0,0);
            mid = mid.addSecs(m_rival.r_searchTime - (m_rival.r_searchTime - now.secsTo(m_rival.r_end)));
            ret = mid.toString("HH:mm:ss");
        }
    }

    return(ret);
}

QString chAccount::traitor(const QString field) const
{
    QString ret;

    if(field == "currentTime") {
        int val = -1;
        if(m_traitorWorker) val = 240 - m_traitorWorker->currentKWZ();
        ret = QString("%1").arg(val);
    } else if(field == "timeString") {
        int val = -1;
        if(m_traitorWorker) {
            val = m_traitorWorker->currentKWZ();
            QTime mid = QTime(0,0,0,0);
            mid = mid.addSecs(val);
            ret = mid.toString("HH:mm:ss");
        }
    } else {
        if(m_traitorWorker) ret = m_traitorWorker->traitor(field);
    }

    return(ret);
}

void chAccount::toggle(const QString option, const bool on)
{
    if(option == "account") {
        m_config.bot = on;

        if(on) {
            if(m_infoWorker) m_infoWorker->setOn();
            if(m_fightWorker && m_config.opponents) {
                if(m_fightWorker->opponent() == "") chooseOpponent();
                m_fightWorker->setOn();
            }
            if(m_traitorWorker && m_config.traitor) m_traitorWorker->setOn();
        } else {
            if(m_infoWorker) m_infoWorker->setOff();
            if(m_fightWorker) m_fightWorker->setOff();
            if(m_traitorWorker) m_traitorWorker->setOff();
        }

    } else if(option == "opponents") {
        m_config.opponents = on;
        if(on) {
            if(m_fightWorker && isActive()) {
                if(m_fightWorker->opponent() == "") chooseOpponent();
                m_fightWorker->setOn();
            }
        } else {
            if(m_fightWorker) m_fightWorker->setOff();
        }

    } else if(option == "rivals") {
        m_config.rivals = on;
        if(on) {
            QTimer::singleShot(50, this, SLOT(fightRival()));
        } else {
        }

    } else if(option == "diary") {
        m_config.diary = on;
        if(on) {
        } else {
        }

    } else if(option == "traitor") {
        m_config.traitor = on;
        if(m_traitorWorker) {
            if(on) {
                if(isActive()) m_traitorWorker->setOn();
            } else {
                m_traitorWorker->setOff();
            }
        }

    } else if(option == "autosave") {
        m_config.autosaver = on;
        if(on) {
        } else {
        }

    }
    qDebug() << "chAccount::toggle:" << option << on;
}

void chAccount::click(const QString button)
{
    if(button == "opponent") {
        chooseOpponent();
    } else if(button == "reloadVip") {
        if(m_infoWorker) {
            m_gangstersTable->clear();
            m_infoWorker->fightsVip();
        }
    } else {
        qDebug() << "chAccount::click" << button;
    }
}

void chAccount::chooseOpponent()
{
    if(!m_fightWorker) return;
    QString pid;
    pid = m_gangstersTable->randomPlayer(m_currentRace);
    if(pid == "") {
        if(m_currentRace != "0") {
            // ToDo:
            // Überfall und Tagebuch beachten
            raceChanged("0");
            return;
        }
    }
    m_dataParser->clearLastResult();
    m_fightWorker->setOpponent(pid);
    m_currentOpponent = pid; // rausnehmen?
    //m_mustReload = true;

    //qDebug() << "chAccount::chooseOpponent:" << pid << m_gangstersTable->getPlayerData(pid,"name") << m_gangstersTable->getPlayerData(pid,"fightsDone");
}

void chAccount::raceChanged(const QString race)
{
    if(m_currentRace == race) return;
    m_currentRace = race;
    chooseOpponent();
}

QString chAccount::workingTitle() const
{
    return(m_fightWorker->pageTitle());
}

void chAccount::heistWork()
{
    if(!m_heistActive) return;
    if(m_workingPage) {
        QUrl url("http://www.chicago1920.com/battleNpc/start/333/1");
        //m_workingPage->mainFrame()->load(url);
        qDebug() << "heist:" << url;
    }
}

// fight-result
void chAccount::heistGetResults(const QString result)
{
    QByteArray data;
    data.append(result);
    QJsonDocument json = QJsonDocument::fromJson(data);
    qDebug() << "chAccount::heistGetResults\n" << json.toJson();
}

void chAccount::getBattleEventData(const QString data)
{
    qDebug() << "chAccount::getBattleEventData" << data;
    m_dataParser->getBattleEventData(data);
}

// kampfbericht
void chAccount::fightData(const QString fight)
{
    qDebug() << "chAccount::fightData" << fight;
    m_dataParser->fightData(fight);
}

// fight-result
void chAccount::getResults(const QString result)
{
    m_dataParser->getResults(result);

    QJsonDocument json = m_dataParser->lastResult();
    QJsonObject opponent = json.object().value("opponent").toObject();

    QString pid = opponent.value("id").toString();

    QJsonObject fightTime = json.object().value("aTime").toObject();
    QJsonObject results = json.object().value("results").toObject();
    QJsonObject fights = json.object().value("fights").toObject();


    QDateTime happens = QDateTime::fromTime_t(fightTime.value("0").toDouble());

    int done = fights.value("doneFights").toInt() + 1;
    int max = fights.value("maxFights").toInt();

    if(done >= max) {
        //m_gangstersTable->setPlayerData(pid, "fightlimit", "true");
        if(m_infoWorker) m_infoWorker->fightsVip();
        chooseOpponent();
    }

    bool ok = results.value("fightWasWon").toBool(false);
    if(!ok) {
        // ToDo:
        // diesen Gegner aus der Liste streichen
        // und neuen wählen
    }

    QJsonObject player1(results.value("p1").toObject());
    QJsonObject player2(results.value("p2").toObject());

    if(m_config.bot) { // are we active?
        if(m_fightWorker) {
            if(m_config.autosaver) { // should we save the money?
                if(m_fightWorker->status("dollar") > 2486) {
                    if(m_infoWorker) m_infoWorker->saveMax();
                }
            }
        }
    }
    //QJsonDocument debug(player2);

    //qDebug() << "[chAccount::getResults]\nall" << json.toJson();
    setDayOfYear(happens.date().dayOfYear());

    qDebug() << "[chAccount::getResults] day:" << happens.date().dayOfYear() << m_gangstersTable->getPlayerData(pid, "name")
             << m_gangstersTable->getPlayerData(pid, "fightsDone")
             << m_gangstersTable->getPlayerData(pid, "fightsMax")
             << ok << m_fightWorker->currentKWZ();

    //qDebug() << "\t[chAccount::getResults]" << opponent.toVariantMap();
    //qDebug() << "\t[chAccount::getResults]" << fights.toVariantMap();
    //qDebug() << "\t[chAccount::getResults]\n" << json.toJson();
    m_mustReload = true;
    //m_infoWorker->fightsStart();

    // for debugging only:
    //if(done >= 1) m_isActive = false;
}

int chAccount::kwz()
{
    int ret = -1;
    if(m_fightWorker) ret = m_fightWorker->currentKWZ();
    return(ret);
}

void chAccount::battleData(const QString result)
{
    QByteArray data;
    data.append(result);
    QJsonDocument json = QJsonDocument::fromJson(data);
    qDebug() << "\tchAccount::battleData\n" << json.toJson() << "\n";
}


void chAccount::dataReady(QNetworkReply* reply)
{
    if(reply != p_lastReply) {

        if(!p_replyData.isEmpty()) {
            // ToDo: auswerten
        }
        p_replyData.truncate(0);
        p_lastReply = reply;

    }
    p_replyData.append(reply->peek(reply->bytesAvailable()));
    //qDebug() << "\t[chAccount::dataReady ]" << reply << reply->url().path() << reply->bytesAvailable();
}

void chAccount::parseCharacters(WebPage* page,const QStringList paths)
{
    QWebFrame* mainFrame = page->mainFrame();

    if(paths.count() == 1) {

        m_nameValue = mainFrame->findFirstElement(".char-mydata").toPlainText().trimmed();
        m_levelValue = mainFrame->findFirstElement(".char-mydatal").toPlainText().trimmed();

    }
}

void chAccount::setDayOfYear(int day)
{
    if(m_currentDay == day) return;
    m_currentDay = day;
    // Tages-Variablen zurücksetzen
    if(m_traitorWorker) m_traitorWorker->midnightReset();
    m_gangstersTable->midnightReset();
    if(m_infoWorker) m_infoWorker->fightsVip();
    chooseOpponent();
}

void chAccount::midnightReset()
{
}

void chAccount::fightRival()
{
    if(m_rival.r_id.isEmpty()) return;
    if(!m_fightWorker) return;
    if(!m_config.rivals) return;
    if(m_rival.r_end > QDateTime::currentDateTime()) {
        QTimer::singleShot(2+QDateTime::currentDateTime().secsTo(m_rival.r_end)*1000, this, SLOT(fightRival()));
        return;
    }
    m_fightWorker->setRival(m_rival.r_id);
    //qDebug() << "chAccount::fightRival" << m_rival.r_id;
}

void chAccount::rivalsData(const QString data)
{
    QByteArray cdata;
    cdata.append(data);
    QJsonDocument json = QJsonDocument::fromJson(cdata);
    QJsonArray rivals = json.array();
    for(QJsonArray::const_iterator i = rivals.constBegin(); i != rivals.end(); ++i) {
        QJsonObject rival = (*i).toObject();
        if(rival.isEmpty()) continue;
        if(rival.value("countdown").toBool()) {

            QString rivalId = QString("%1").arg(rival.value("search_id").toInt());
            if(rivalId != m_rival.r_id) {
                m_rival.r_id = rivalId;
                int timeTo = rival.value("timer").toInt(); // countdown secs
                m_rival.r_end = QDateTime::currentDateTime().addSecs(timeTo);
                int timeAll = (rival.value("bonus").toObject().value("minutes").toInt() * 60);
                m_rival.r_searchTime = timeAll;
                QTimer::singleShot(((timeTo+1)*1000), this, SLOT(fightRival()));
            }
/*
            QJsonDocument debug(rival);
            qDebug() << debug.toJson()
                     << "chAccount::rivalsData" << m_rival.r_end.toString() << "sek.\n"
                     << m_rival.r_searchTime << "sek. gesamt\n";
*/
        } else if(rival.value("attack").toBool()) {

            QString rivalId = QString("%1").arg(rival.value("search_id").toInt());
            if(rivalId != m_rival.r_id) {
                m_rival.r_id = rivalId;
                m_rival.r_searchTime = (rival.value("bonus").toObject().value("minutes").toInt() * 60);
                QTimer::singleShot(50, this, SLOT(fightRival()));
            }
/*
            QJsonDocument debug(rival);
            qDebug() << debug.toJson()
                     << "chAccount::rivalsAttack:"
                     << m_rival.r_searchTime << "sek. gesamt\n";
*/
        } else {
            QJsonDocument debug(rival);
            //qDebug() << "sonstiger Rivale:\n" << debug.toJson();
        }
    }
    //qDebug() << "chAccount::rivalsData" << json.toJson();
}

void chAccount::parseRivals(QWebFrame* mainFrame,const QStringList paths)
{
    if(!paths.count()) return;
    if(paths.count() == 1) return;
    if(paths.at(1) != "index") return;

    QString question = "account.rivalsData(JSON.stringify(slavelist));";
    mainFrame->evaluateJavaScript(question);
}

void chAccount::loadFinished(WebPage* page)
{
    QWebFrame* mainFrame = page->mainFrame();
    QUrl url = mainFrame->url();
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);

    if(!paths.count()) return; // nothing to do

    m_dataParser->setNetworkAccessManager(page->networkAccessManager());

    if(m_infoWorker == NULL) {
        m_infoWorker = new infoWorker(page->networkAccessManager());
        m_infoWorker->setOn();
    }

    if(m_fightWorker == NULL) {
        m_fightWorker = new fightWorker(page->networkAccessManager());
        connect(m_fightWorker, SIGNAL(fightsDone()), this, SLOT(chooseOpponent()));
        connect(m_fightWorker, SIGNAL(fightCooldown(int)), this, SLOT(fightDone(int)));
    }

    if(m_traitorWorker == NULL) {
        m_traitorWorker = new traitorWorker();
        m_traitorWorker->setNetworkAccessManager(page->networkAccessManager());
        m_traitorWorker->setOff();
    }


    if(QString("characters") == paths.at(0)) {

        parseCharacters(page, paths);

    } else if(QString("rival") == paths.at(0)) {

        parseRivals(mainFrame, paths);

    }

    QString logString;
    QDateTime now = QDateTime::currentDateTimeUtc();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append("  chAccount::loadFinished (" + url.path());
    logString.append(") '" + mainFrame->title() + "'");
    //qDebug() << logString;
}

void chAccount::checkMoney()
{
    QString pid = m_gangstersTable->accountGangster();
    if(pid.isEmpty()) return;
    if(m_config.bot && m_config.autosaver) { // are we active?
        if(m_gangstersTable->getPlayerData(pid, "dollar").toInt() > m_config.maxMoney) {
            if(m_infoWorker) m_infoWorker->saveMax();
        }
    }
}

void chAccount::parseFights(const QStringList paths)
{
    if(paths.count() < 2) return;

    if(paths.at(1) == QString("start")) {

        if(paths.count() == 2) {
            // nur, wenns vom fightWorker kommt...
            QJsonDocument json = m_dataParser->lastResult();
            QJsonObject fights = json.object().value("fights").toObject();
            int done = fights.value("doneFights").toInt() + 1;
            int max = fights.value("maxFights").toInt();
            if(done >= max) {
                //m_gangstersTable->setPlayerData(pid, "fightlimit", "true");
                //chooseOpponent();
            }
            //qDebug() << "chooseOpponent?";
        }

    } else if(paths.at(1) == QString("fightData")) {

        m_dataParser->fightData(QString(p_replyData));

    } else if(paths.at(1) == QString("results")) {

    } else if(paths.at(1) == QString("getResults")) {

        // Fight-Result
        if(paths.count() == 3) {
            m_dataParser->getResults(QString(p_replyData));
            QJsonDocument json = m_dataParser->lastResult();
            QJsonObject fightTime = json.object().value("aTime").toObject();
            QDateTime happens = QDateTime::fromTime_t(fightTime.value("0").toDouble());
            setDayOfYear(happens.date().dayOfYear());

            QJsonObject opponent = json.object().value("opponent").toObject();
            QString pid = opponent.value("id").toString();

            if(m_fightWorker) {
                // m_fightWorker->opponent setzen.
                if(pid != m_fightWorker->opponent()) {
                    m_fightWorker->setOpponent(pid);
                }
            }

            checkMoney();

            QJsonObject fights = json.object().value("fights").toObject();
            int done = fights.value("doneFights").toInt() + 1;
            int max = fights.value("maxFights").toInt();
            if(done >= max) {
                m_gangstersTable->setPlayerData(pid, "fightlimit", "true");
                QTimer::singleShot(60000, this, SLOT(chooseOpponent()));
            }

            QJsonObject results = json.object().value("results").toObject();
            bool ok = results.value("fightWasWon").toBool(false);
            if(!ok) {
                // ToDo:
                // diesen Gegner aus der Liste streichen
                // und neuen wählen
            }

            m_mustReload = true;
        }

    } else if(paths.at(1) == QString("waitFight")) {

    } else if(paths.at(1) == QString("vip")) {

    } else if(paths.at(1) == QString("opponentsListJson")) {

        // Angriffs-Liste
        m_dataParser->opponentsListJson(QString(p_replyData));
        //qDebug() << p_replyData;

    } else if(paths.at(1) == QString("enemysListJson")) {

        // Vip-Liste
        m_dataParser->enemysListJson(QString(p_replyData));
        // ToDo: Gangster-ID auslesen.

        if(m_fightWorker && m_fightWorker->opponent() == "") {
            chooseOpponent();
        }
        //qDebug() << p_replyData;
        //qDebug() << "[chAccount::enemysListJson] gangsters in list:" << m_gangstersTable->countGangsters();

    }
}

void chAccount::loadFinished(QNetworkReply* reply)
{
    QStringList paths = reply->url().path().split("/",QString::SkipEmptyParts);

    if(!paths.count()) return; // nothing to do, login evtl?

    if(!p_replyData.isEmpty() && p_replyData.trimmed().startsWith("<!DOCTYPE")) m_dataParser->parseHtml(p_replyData.trimmed());

    if(paths.at(0) == QString("fights")) {

        parseFights(paths);

    } else if(paths.at(0) == QString("rivalNpc")) {

        if(paths.count() == 3) {
            if(paths.at(1) == QString("getResults")) {
                qDebug() << p_replyData << "\n\t[chAccount::loadFinished] (rival)";
                checkMoney();
            }
        }

    } else if(paths.at(0) == QString("battleNpc")) {

        if(paths.at(1) == QString("getResults")) {

            qDebug() << p_replyData << "\n\t[chAccount::loadFinished] (Npc)";
            checkMoney();

        } else if(paths.at(1) == QString("start")) {

            if(m_traitorWorker) {
                int row = 0, col = 0;
                if(paths.count() == 4) { // battleNpc/start/1/1
                    col = paths.at(2).toInt();
                    row = paths.at(3).toInt();
                    if(paths.at(2).toInt() > 0 && paths.at(3).toInt() > 0) {
                        m_traitorWorker->setTraitor(row,col);
                    }
                } else if(paths.count() == 3) {
                    col = paths.at(2).toInt();
                    row = 1;
                    m_traitorWorker->setTraitor(row,col);
                }
            }

        }

    } else if(paths.at(0) == QString("cq")) {

        // ToDo: Modus auslesen (Abstimmung, Viertel-Kampf, LMS)
        //qDebug() << p_replyData;

    } else if(paths.at(0) == QString("challenge")) {

            // diary data
        if(paths.count() > 1 && paths.at(1) == QString("diary_data")) {
            m_dataParser->diaryData(QString(p_replyData));
        }

    } else if(paths.at(0) == QString("patenvilla")) {

        // patenvilla data
        if(paths.count() > 2 && paths.at(1) == QString("getData")) {
            m_dataParser->patenvillaData(QString(p_replyData));
        }

    } else if(paths.at(0) == QString("cbi")) {

        if(paths.count() > 1 && paths.at(1) == QString("fightData")) {
            m_dataParser->fightData(QString(p_replyData));
        }

    } else if(paths.at(0) == QString("games")) {

        if(paths.count() > 1 && paths.at(1) == QString("getRouletteResult")) {
            //qDebug() << "\nRouletteResult:" << p_replyData;
        }

    } else if(paths.at(0) == QString("battle")) {

        if(paths.count() > 1 && paths.at(1) == QString("getBattleEventData")) {
            m_dataParser->getBattleEventData(QString(p_replyData));
        }
    }

    qDebug() << "\t[chAccount::loadFinished]" << reply << reply->url().path() << p_replyData.size();
}
