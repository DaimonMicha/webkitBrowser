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
    m_gangstersTable(new chPlayerTable)
{
    //m_cookieValue = cookie;
    m_config.bot = false;

    m_infoWorker = NULL;
    m_fightWorker = NULL;
    m_currentRace = "0";

    m_cityMap["1"]="Dunning";
    m_cityMap["2"]="Uptown";
    m_cityMap["3"]="Lawndale";
    m_cityMap["4"]="Loop";
    m_cityMap["5"]="Newcity";
    m_cityMap["6"]="Woodlawn";
    m_cityMap["7"]="Ashburn";
    m_cityMap["8"]="Southdeering";

}

// vip-liste
void chAccount::enemysListJson(const QString enemys)
{
    QByteArray data;
    data.append(enemys);
    QJsonDocument json = QJsonDocument::fromJson(data);
    QJsonArray users = json.object().value("list").toArray();

    QJsonArray::const_iterator i = users.constBegin();
    for(i = users.constBegin(); i != users.end(); ++i) {
        QJsonObject player = (*i).toObject();

        QString pid = player.value("id").toString();
        m_gangstersTable->setPlayerData(pid, "name", player.value("name").toString());
        m_gangstersTable->setPlayerData(pid, "level", player.value("level").toString());
        m_gangstersTable->setPlayerData(pid, "race_id", m_cityMap.key(player.value("race").toString()));
        if(player.value("fightlimit").toBool()) m_gangstersTable->setPlayerData(pid, "fightlimit", "true");
        else m_gangstersTable->setPlayerData(pid, "fightlimit", "false");
        //qDebug() << "readVip-fightlimit:" << player.value("fightlimit");
        m_gangstersTable->setPlayerData(pid, "rang", player.value("rang").toString());
        m_gangstersTable->setPlayerData(pid, "win", player.value("win").toString());
        m_gangstersTable->setPlayerData(pid, "lost", player.value("lost").toString());
        m_gangstersTable->setPlayerData(pid, "gold", player.value("gold").toString());
        m_gangstersTable->setPlayerData(pid, "coded_id", player.value("profil").toString().split("/",QString::SkipEmptyParts).last());
        //qDebug() << "\n" << player.value("profil").toString().split("/",QString::SkipEmptyParts).last();
        //qDebug() << "\n" << player.toVariantMap().keys();
    }

    if(m_fightWorker && isActive() && m_fightWorker->opponent() == "") {
        chooseOpponent();
    }

    //qDebug() << "chAccount::enemysListJson\n" << json.toJson();
             //<< "\n" << json.object().value("list").toArray();
}

QString chAccount::opponent(const QString field) const
{
    QString pid = m_fightWorker->opponent();
    //qDebug() << "chAccount::opponent" << pid.toInt() << field << m_gangstersTable->getPlayerData(pid, field);
    return(m_gangstersTable->getPlayerData(pid, field));
}

void chAccount::chooseOpponent()
{
    if(!m_fightWorker) return;
    QString pid;
    pid = m_gangstersTable->randomPlayer(m_currentRace);
    if(pid == "") {
        if(m_currentRace != "0") {
            raceChanged("0");
            return;
        }
    }
    m_fightWorker->setOpponent(pid);
    m_currentOpponent = pid; // rausnehmen?

    //qDebug() << "chAccount::chooseOpponent:" << pid << m_gangstersTable->getPlayerData(pid,"name") << m_gangstersTable->getPlayerData(pid,"fightsDone");
}

void chAccount::toggle(const QString option, const bool on)
{
    if(option == "account") {

        if(isActive()) {
            m_config.bot = false;
            if(m_infoWorker) m_infoWorker->setOff();
            if(m_fightWorker) m_fightWorker->setOff();
        } else {
            m_config.bot = true;
            if(m_infoWorker) m_infoWorker->setOn();
            if(m_fightWorker) {
                m_fightWorker->setOn();
                if(m_fightWorker->opponent() == "") chooseOpponent();
            }
        }

    } else if(option == "rivals") {
        m_config.rivals = on;
        if(on) {
        } else {
        }

    } else if(option == "diary") {
        m_config.diary = on;
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
    }
    //qDebug() << "chAccount::click" << button;
}

void chAccount::raceChanged(const QString race)
{
    if(m_currentRace == race) return;
    m_currentRace = race;
    chooseOpponent();
    //qDebug() << "chAccount::raceChanged: " << race + ": " + m_cityMap.value(race);
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
}

// angriff-liste
void chAccount::opponentsListJson(const QString opponents)
{
    QByteArray data;
    data.append(opponents);
    QJsonDocument json = QJsonDocument::fromJson(data);

    qDebug() << "chAccount::opponentsListJson" << json.object().value("list").toArray();
}

// kampfbericht
void chAccount::fightData(const QString fight)
{
    QByteArray data;
    data.append(fight);
    QJsonDocument json = QJsonDocument::fromJson(data);
    QJsonObject player = json.object().value("p1").toObject();
    m_nameValue = player.value("name").toString();
    m_levelValue = player.value("level").toString();
    QJsonValue results = json.object().value("results");
/*
    if(json.object().value("won").toDouble() == 1) {
        // gewonnen! player in model updaten
        player = json.object().value("p2").toObject();
        QStandardItem *playerItem;
        QList<QStandardItem *> rows = m_opponentsModel->findItems(m_currentOpponent);
        if(0 == rows.count()) {
            QList<QStandardItem *> items;
            playerItem = new QStandardItem(m_currentOpponent);
            items.append(playerItem);
            for(int c = 1; c < m_opponentsModel->columnCount(); ++c) {
                items.append(new QStandardItem());
            }
            m_opponentsModel->appendRow(items);
        } else {
            playerItem = rows.at(0);
        }
        int row = playerItem->row();
        // update
        m_opponentsModel->item(row, 1)->setText(player.value("name").toString());
        m_opponentsModel->item(row, 2)->setText(QString("%1").arg(player.value("life").toDouble()));
        m_opponentsModel->item(row, 3)->setText(QString("%1").arg(player.value("level").toDouble()));
        m_opponentsModel->item(row, 4)->setText(player.value("race").toString());
    }
*/
    qDebug() << "chAccount::fightData" << json.object().keys() << "\n" << results.toArray()
             << "\n" << m_currentOpponent << player << json.toJson();
}

// fight-result
void chAccount::getResults(const QString result)
{
    QByteArray data;
    data.append(result);
    QString pid;
    QJsonDocument json = QJsonDocument::fromJson(data);
    QJsonObject opponent = json.object().value("opponent").toObject();
    pid = opponent.value("id").toString();

    QJsonObject fightTime = json.object().value("aTime").toObject();
    QDateTime happens = QDateTime::fromTime_t(fightTime.value("0").toDouble());

    QJsonObject fights = json.object().value("fights").toObject();

    int done = fights.value("doneFights").toInt() + 1;
    int max = fights.value("maxFights").toInt();

    m_gangstersTable->setPlayerData(pid, "fightsDone", QString("%1").arg(done));
    m_gangstersTable->setPlayerData(pid, "fightsMax",  QString("%1").arg(max));
    m_gangstersTable->setPlayerData(pid, "coded_id", opponent.value("codedId").toString());
    m_gangstersTable->setPlayerData(pid, "level", opponent.value("level").toString());
    m_gangstersTable->setPlayerData(pid, "name", opponent.value("name").toString());
    m_gangstersTable->setPlayerData(pid, "race_id", opponent.value("race_id").toString());
/*
    if((done + 1) == max) {
        if(m_infoWorker) m_infoWorker->fightsVip();
    }
*/
    if(done >= max) {
        m_gangstersTable->setPlayerData(pid, "fightlimit", "true");
        if(m_infoWorker) m_infoWorker->fightsVip();
        chooseOpponent();
    }

    QJsonObject results = json.object().value("results").toObject();
    bool ok = results.value("fightWasWon").toBool(false);
    if(!ok) {
        // ToDo:
        // diesen Gegner aus der Liste streichen
        // und neuen wählen
    }

    QJsonObject player1(results.value("p1").toObject());
    QJsonObject player2(results.value("p2").toObject());

    m_gangstersTable->setPlayerData(pid, "maxLP", player2.value("maxLp").toString());
    m_gangstersTable->setPlayerData(pid, "platting", player2.value("platting").toString());
    m_gangstersTable->setPlayerData(pid, "minDamage", player2.value("fightDamage").toObject().value("from").toString());
    m_gangstersTable->setPlayerData(pid, "maxDamage", player2.value("fightDamage").toObject().value("to").toString());
    m_gangstersTable->setPlayerData(pid, "magicDamage", player2.value("magicDamage").toString());

    //QJsonDocument debug(player2);

    //qDebug() << "[chAccount::getResults]\nall" << json.toJson();
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

// Tages-Aufgaben
void chAccount::diaryData(const QString diary)
{
    QByteArray data;
    data.append(diary);
    QJsonDocument json = QJsonDocument::fromJson(data);
    //qDebug() << "\tchAccount::diaryData" << json.toJson();
    // aufgaben
    QJsonArray aufgaben = json.object().value("aufgaben").toArray();
    for(QJsonArray::const_iterator i = aufgaben.constBegin(); i != aufgaben.end(); ++i) {
        QJsonObject aufgabe = (*i).toObject();
        if(aufgabe.value("active").toString() == QString("1")) {
            if(aufgabe.value("type").toString() == QString("fights_race")) {
                QJsonDocument debug(aufgabe);
                //qDebug() << "chAccount::diaryData\n" << debug.toJson();
            }
        }
    }
    QJsonDocument debug(aufgaben);
    //qDebug() << "\tchAccount::diaryData\n" << debug.toJson();
}

// Patenvilla Daten
void chAccount::patenvillaData(const QString villa)
{
    QByteArray data;
    data.append(villa);
    QJsonDocument json = QJsonDocument::fromJson(data);
    QJsonArray liste = json.object().value("list").toArray();
    for(QJsonArray::const_iterator i = liste.constBegin(); i != liste.end(); ++i) {
        QJsonObject feature = (*i).toObject();
        if(!feature.isEmpty() && feature.value("is_activate").toBool()) {
            QJsonDocument debug(feature);
            qDebug() << "\tchAccount::patenvillaData\n" << debug.toJson() << "\n";
        }
    }
    //QJsonDocument debug(liste);
    //qDebug() << "\tchAccount::patenvillaData\n" << debug.toJson() << "\n";
}

void chAccount::loadFinished(QNetworkReply* reply)
{
    QUrl url = reply->url();
    QString path = url.path();
    if(path.endsWith(".swf")) return;
    if(path.endsWith(".jpg")) return;
    if(path.endsWith(".png")) return;
    if(path.endsWith(".gif")) return;
    if(path.endsWith(".ico")) return;
    if(path.endsWith(".cur")) return;
    if(path.endsWith(".css")) return;
    if(path.endsWith(".js")) return;
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);
    if(QString("fights") == paths.at(0)) { //  || QString("rivalNpc") == paths.at(0)
        if(paths.count() == 3) {
            if(QString("start") == paths.at(1)) {
                //m_currentOpponent = paths.at(2);
                if(m_fightWorker) {
                    if(paths.at(2) != m_fightWorker->opponent()) {
                        //qDebug() << "new current Opponent:" << paths.at(2) << m_fightWorker->opponent();
                        m_fightWorker->setOpponent(paths.at(2));
                    }
                }
            } else if(QString("getResults") == paths.at(1)) {
                //if(m_infoWorker) m_infoWorker->fightsStart();
            }
        } else if(path == "/fights/busy") {
            //if(m_infoWorker) m_infoWorker->fightsStart();
        }
    }
    //qDebug() << "\tchAccount::replyFinished" << path;
}

void chAccount::parseCharacters(WebPage* page,const QStringList paths)
{
    QWebFrame* mainFrame = page->mainFrame();

    if(paths.count() == 1) {

        m_nameValue = mainFrame->findFirstElement(".char-mydata").toPlainText().trimmed();
        m_levelValue = mainFrame->findFirstElement(".char-mydatal").toPlainText().trimmed();

    }
}

void chAccount::parseChallenge(WebPage* page,const QStringList paths)
{
    QWebFrame* mainFrame = page->mainFrame();

    if(paths.count() > 1) {

        if(QString("diary") == paths.at(1)) {
            QVariant result = mainFrame->evaluateJavaScript("new Ajax.Request('/challenge/diary_data',{asynchronous: true,method: 'POST',dataType: 'json',onSuccess: function(result){account.diaryData(result.responseText);}});");
        }

    }
}

void chAccount::parseCbi(WebPage* page,const QStringList paths)
{
    //QWebFrame* mainFrame = page->mainFrame();

    if(paths.count() > 1) {

        if(QString("fight") == paths.at(1)) {
            //result = mainFrame->evaluateJavaScript("new Ajax.Request('/cbi/fightData',{asynchronous: true,method: 'POST',dataType: 'json',onSuccess: function(result){account.fightData(result.responseText);}});");
        }

    }
}

void chAccount::parseFights(WebPage* page,const QStringList paths)
{
    if(paths.count() < 2) return;

    QWebFrame* mainFrame = page->mainFrame();
    QVariant result;

    if(QString("start") == paths.at(1)) {
        //der passt:
        //result = mainFrame->evaluateJavaScript("new Ajax.Request('/fights/opponentsListJson',{asynchronous: true,method: 'POST',dataType: 'json',onSuccess: function(result){account.opponentsListJson(result.responseText);}});");
        if(paths.count() == 3) {
            m_currentOpponent = paths.at(2);
            qDebug() << "current Opponent:" << m_currentOpponent;
        }

    } else if(QString("fight") == paths.at(1)) {

        //result = mainFrame->evaluateJavaScript("new Ajax.Request('/fights/fightData',{asynchronous: true,method: 'POST',dataType: 'json',onSuccess: function(result){account.fightData(result.responseText);}});");

    } else if(QString("results") == paths.at(1)) {

        // todo: wenn(m_isActive == true) flag setzen, um auf den reload  zu warten und /fights in einer versteckten QWebPage aufrufen...
        if(paths.count() > 2) {
            QString question = "new Ajax.Request('/fights/getResults/";
            question.append(paths.at(2));
            question.append("',{asynchronous: false,method: 'POST',dataType: 'json',onSuccess: function(result){account.getResults(result.responseText);}});");
            //qDebug() << "results:" << question;
            //result = mainFrame->evaluateJavaScript(question);
            //QWebElement title = mainFrame->findFirstElement("title");
            //qDebug() << "chAccount::parseFights (results): " << workingTitle();
            //if(m_infoWorker) m_infoWorker->fightsStart();
        }
        //result = mainFrame->evaluateJavaScript("new Ajax.Request('/fights/opponentsListJson',{asynchronous: true,onSuccess: function(result){window.alert(result.responseText);}});");

    } else if(QString("waitFight") == paths.at(1)) {

        //result = mainFrame->evaluateJavaScript("window.alert(\"Account: ...warten mit \" + window.location.href + \" Whisky.\");");

    } else if(QString("vip") == paths.at(1)) {

        result = mainFrame->evaluateJavaScript("new Ajax.Request('/fights/enemysListJson',{asynchronous: true,method: 'POST',dataType: 'json',onSuccess: function(result){account.enemysListJson(result.responseText);}});");

    }

}

void chAccount::loadFinished(WebPage* page)
{
    QWebFrame* mainFrame = page->mainFrame();
    QUrl url = mainFrame->url();
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);

    if(!paths.count()) return; // nothing to do, login evtl?

    if(m_fightWorker == NULL) {
        m_fightWorker = new fightWorker();
        m_fightWorker->setNetworkAccessManager(page->networkAccessManager());
        connect(m_fightWorker, SIGNAL(fightDataReady(QString)), this, SLOT(fightData(QString)));
        connect(m_fightWorker, SIGNAL(fightResults(QString)), this, SLOT(getResults(QString)));
        connect(m_fightWorker, SIGNAL(fightsDone()), this, SLOT(chooseOpponent()));
    }

    if(m_infoWorker == NULL) {
        m_infoWorker = new infoWorker();
        m_infoWorker->setNetworkAccessManager(page->networkAccessManager());
        connect(m_infoWorker, SIGNAL(diarydata(QString)), this, SLOT(diaryData(QString)));
        connect(m_infoWorker, SIGNAL(enemysList(QString)), this, SLOT(enemysListJson(QString)));
        connect(m_infoWorker, SIGNAL(patenvilla(QString)), this, SLOT(patenvillaData(QString)));
        //connect(m_infoWorker, SIGNAL(cooldownEnd()), m_fightWorker, SLOT(startFight()));
        m_infoWorker->setOn();
    }


    if(QString("characters") == paths.at(0)) {

        parseCharacters(page, paths);

    } else if(QString("challenge") == paths.at(0)) {

        parseChallenge(page, paths);

    } else if(QString("cbi") == paths.at(0)) {

        parseCbi(page, paths);

    } else if(QString("fights") == paths.at(0)) {

        parseFights(page, paths);

    } else if(QString("rival") == paths.at(0)) {

        parseRivals(mainFrame, paths);

    } else if(QString("patenvilla") == paths.at(0)) {

        if(paths.count() == 1) {
            QString question = "new Ajax.Request('/patenvilla/getData/";

            question.append(QCryptographicHash::hash("ghdh67TZGHb56fgsdfkk0",QCryptographicHash::Md5).toHex());
            question.append("',{asynchronous: false,method: 'POST',dataType: 'json',onSuccess: function(result){account.getResults(result.responseText);}});");
            //qDebug() << "results:" << question;
            //result = mainFrame->evaluateJavaScript(question);
            //QWebElement title = mainFrame->findFirstElement("title");
            //qDebug() << "chAccount::loadFinished (patenvilla): " << workingTitle() << question;
        }
        //result = mainFrame->evaluateJavaScript("new Ajax.Request('/fights/opponentsListJson',{asynchronous: true,onSuccess: function(result){window.alert(result.responseText);}});");

    } else if(QString("battle") == paths.at(0)) {

        //if(paths.count() > 2) {

            //if(QString("index") == paths.at(1) && QString("map") == paths.at(2)) {
                //result = mainFrame->evaluateJavaScript("new Ajax.Request('/battle/getBattleEventData',{asynchronous: true,method: 'POST',onSuccess: function(result){account.getBattleEventData(result.responseText);}});");
            //}

        //}

    } else if(QString("battleNpc") == paths.at(0)) {

        if(paths.count() > 1) {

        }

    } else if(QString("secretfiles") == paths.at(0)) {

        if(paths.count() > 1) {

            if(QString("index") == paths.at(1)) {
                //mainFrame->evaluateJavaScript(" alert('Hello World!'); ");
            }

        }

    }

/*
    if(pluginDiv.isNull()) {

        QWebElement body = mainFrame->findFirstElement("body");
        body.appendInside("<div style=\"position: absolute; float: left; width: 155px; height: 95%; top: 15px; padding-left: 5px; text-align: left; border: 1px solid yellow;\" id=\"accountPlugin\">&nbsp;</div>");
        pluginDiv = mainFrame->findFirstElement("#accountPlugin");
        if(!m_nameValue.isEmpty()) pluginDiv.appendInside("<h1 id='playerName' style='cursor: pointer;'>"+m_nameValue+"</h1>");
        if(m_levelValue != 0) pluginDiv.appendInside("<h2>&nbsp;&nbsp;&nbsp;Level:&nbsp;"+m_levelValue+"</h2>");
        // insert last opponents
        pluginDiv.appendInside("<div style=\"font-size: 10pt; position: absolute; bottom: 10px; width: 143px; height: 70%; background-color:rgba(0, 0, 0, 0.65);\" id=\"lastOpponents\"></div>");
        QWebElement opponents = mainFrame->findFirstElement("#lastOpponents");
        opponents.appendInside("<table width=\"100%\"><tbody id=\"playersTable\"></tbody></table>");
        QWebElement pTable = mainFrame->findFirstElement("#playersTable");
        int start = 0;
        if(m_opponentsModel->rowCount() > 30) start = m_opponentsModel->rowCount()-30;
        for(int row = start;row < m_opponentsModel->rowCount();++row) {
            QStandardItem* item = m_opponentsModel->item(row, 1);
            QString out = "<tr><td>";
            out.append(item->text());
            out.append("</td><td align=\"right\">");
            item = m_opponentsModel->item(row, 2);
            out.append(item->text());
            out.append("</td></tr>");
            pTable.appendInside(out);
        }
        mainFrame->evaluateJavaScript("jQuery('#playerName').click(function(){account.toggle();if(account.isActive()){jQuery(this).css({color:'red'});}else{jQuery(this).css({color:'rgb(253,255,225)'});}});");
        mainFrame->evaluateJavaScript("if(account.isActive()){jQuery('#playerName').css({color:'red'});}else{jQuery('#playerName').css({color:'rgb(253,255,225)'});}");
        //qDebug() << mainFrame->evaluateJavaScript("typeof(account);");

    }
*/

    QString logString;
    QDateTime now = QDateTime::currentDateTimeUtc();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append("  chAccount::loadFinished (" + url.path());
    logString.append(") '" + mainFrame->title() + "'");
    qDebug() << logString;
}

void chAccount::rivalsData(const QString data)
{
    QByteArray cdata;
    cdata.append(data);
    QJsonDocument json = QJsonDocument::fromJson(cdata);
    QJsonArray rivals = json.array();
    for(QJsonArray::const_iterator i = rivals.constBegin(); i != rivals.end(); ++i) {
        QJsonObject rival = (*i).toObject();
        if(!rival.isEmpty() && rival.value("countdown").toBool()) {

            QJsonDocument debug(rival);
            qDebug() << "chAccount::rivalsData" << rival.value("timer").toInt() << "sek.\n"
                     << debug.toJson();
        }
    }
    //qDebug() << "chAccount::rivalsData" << json.toJson();
}

void chAccount::parseRivals(QWebFrame* mainFrame,const QStringList paths)
{
    if(!paths.count()) return;
    if(paths.at(1) != "index") return;

    QString question = "account.rivalsData(JSON.stringify(slavelist));";
    mainFrame->evaluateJavaScript(question);
}
