#include "dataparser.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QWebElement>

#include <QDebug>




dataParser::dataParser(chPlayerTable* pTable,QObject *parent) :
    QObject(parent),
    m_gangstersTable(pTable),
    m_parserPage(new QWebPage())
{
    m_parserPage->setForwardUnsupportedContent(false);
    connect(m_parserPage->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObject()));

    m_cityMap["1"]="Dunning";
    m_cityMap["2"]="Uptown";
    m_cityMap["3"]="Lawndale";
    m_cityMap["4"]="Loop";
    m_cityMap["5"]="Newcity";
    m_cityMap["6"]="Woodlawn";
    m_cityMap["7"]="Ashburn";
    m_cityMap["8"]="Southdeering";
}

void dataParser::parseHtml(const QByteArray html)
{
    //if(!html.startsWith("<!DOCTYPE")) return;
    m_parserPage->mainFrame()->setHtml(QString(html), QUrl("http://www.chicago1920.com/"));

    // read LifeBar
    QString question = "parser.gangsterStatus('currentCrystals', currentCrystals);\n";
    question.append("parser.gangsterStatus('currentGold', currentGold);\n");
    question.append("parser.gangsterStatus('currentStone', currentStone);\n");
    question.append("parser.gangsterStatus('lifeMax', lifeMax);\n");
    question.append("parser.gangsterStatus('lifeCurrent', lifeCurrent);\n");
    question.append("parser.gangsterStatus('fullLifeAfter', fullLifeAfter);\n");
    m_parserPage->mainFrame()->evaluateJavaScript(question);

    QWebElement t = m_parserPage->mainFrame()->findFirstElement("title");
    QString title = t.toPlainText().trimmed();

    qDebug() << "dataParser::parseHtml" << html.length() << "Bytes" << title;
}

void dataParser::gangsterStatus(const QString topic, const QString value)
{
    if(m_gangstersTable->accountGangster().isEmpty()) return;

    QString pid = m_gangstersTable->accountGangster();

    if(topic == "lifeCurrent") {
    } else if(topic == "lifeMax") {
        m_gangstersTable->setPlayerData(pid, "maxLP", value);
    } else if(topic == "fullLifeAfter") {
        int minWait = 1234;
        if(value.toInt() > 0) {
            minWait += value.toInt();
        }
        int maxWait = minWait + 3654;
        //QTimer::singleShot(randInt(minWait,maxWait), this, SLOT(startFight()));

    } else if(topic == "currentCrystals") { // whisky
        m_gangstersTable->setPlayerData(pid, "whisky", value);
    } else if(topic == "currentGold") { // dollar
        m_gangstersTable->setPlayerData(pid, "dollar", value);
    } else if(topic == "currentStone") { // gangsterPunkte
        m_gangstersTable->setPlayerData(pid, "gp", value);
    }
    //qDebug() << "dataParser::gangsterStatus" << (topic + "\t" + value);
}

// Angriffs-Liste
void dataParser::opponentsListJson(const QString opponents)
{
    QByteArray data;
    data.append(opponents);
    QJsonDocument json = QJsonDocument::fromJson(data);

    //qDebug() << "dataParser::opponentsListJson" << json.object().value("list").toArray();
}

// VIP-Liste
void dataParser::enemysListJson(const QString enemys)
{
    QByteArray data;
    data.append(enemys);
    QJsonDocument json = QJsonDocument::fromJson(data);

    // gangster-id auslesen
    m_gangstersTable->setAccountGangster(json.object().value("char_id").toString());

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
        m_gangstersTable->setPlayerData(pid, "rang", player.value("rang").toString());
        m_gangstersTable->setPlayerData(pid, "win", player.value("win").toString());
        m_gangstersTable->setPlayerData(pid, "lost", player.value("lost").toString());
        m_gangstersTable->setPlayerData(pid, "gold", player.value("gold").toString());
        m_gangstersTable->setPlayerData(pid, "coded_id", player.value("profil").toString().split("/",QString::SkipEmptyParts).last());
        m_gangstersTable->setPlayerData(pid, "type", player.value("enemytype").toString());
    }
    //QJsonDocument debug(users);
    //qDebug() << "dataParser::enemysListJson" << json.toJson();
}

// Kampf-Data
void dataParser::fightData(const QString fight)
{
    QByteArray data;
    data.append(fight);
    QJsonDocument json = QJsonDocument::fromJson(data);
    QJsonObject player = json.object().value("p1").toObject();
    //m_nameValue = player.value("name").toString();
    //m_levelValue = player.value("level").toString();
    QJsonValue results = json.object().value("results");
    QJsonDocument debug(json);
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
    qDebug() << "dataParser::fightData" << debug.toJson() << "\n" << results.toArray();
//             << "\n" << json.toJson();
}

// Kampf-Resultat
void dataParser::getResults(const QString result)
{
    QByteArray data;
    data.append(result);
    m_lastResult = QJsonDocument::fromJson(data);
    QJsonObject opponent = m_lastResult.object().value("opponent").toObject();
    QString pid = opponent.value("id").toString();

    QJsonObject fightTime = m_lastResult.object().value("aTime").toObject();
    QDateTime happens = QDateTime::fromTime_t(fightTime.value("0").toDouble());

    QJsonObject fights = m_lastResult.object().value("fights").toObject();

    int done = fights.value("doneFights").toInt() + 1;
    int max = fights.value("maxFights").toInt();

    m_gangstersTable->setPlayerData(pid, "fightsDone", QString("%1").arg(done));
    m_gangstersTable->setPlayerData(pid, "fightsMax",  QString("%1").arg(max));
    m_gangstersTable->setPlayerData(pid, "coded_id", opponent.value("codedId").toString());
    m_gangstersTable->setPlayerData(pid, "level", opponent.value("level").toString());
    m_gangstersTable->setPlayerData(pid, "name", opponent.value("name").toString());
    m_gangstersTable->setPlayerData(pid, "race_id", opponent.value("race_id").toString());

    QJsonObject results = m_lastResult.object().value("results").toObject();
    bool ok = results.value("fightWasWon").toBool(false);
    QJsonObject player1(results.value("p1").toObject());
    QJsonObject player2(results.value("p2").toObject());

    m_gangstersTable->setPlayerData(pid, "maxLP", player2.value("maxLp").toString());
    m_gangstersTable->setPlayerData(pid, "platting", player2.value("platting").toString());
    m_gangstersTable->setPlayerData(pid, "minDamage", player2.value("fightDamage").toObject().value("from").toString());
    m_gangstersTable->setPlayerData(pid, "maxDamage", player2.value("fightDamage").toObject().value("to").toString());
    m_gangstersTable->setPlayerData(pid, "magicDamage", player2.value("magicDamage").toString());

    //qDebug() << player1.toVariantMap();

    //qDebug() << "[dataParser::getResults]\nall" << json.toJson();
    qDebug() << "[dataParser::getResults] day:" << happens.date().dayOfYear() << m_gangstersTable->getPlayerData(pid, "name")
             << m_gangstersTable->getPlayerData(pid, "fightsDone")
             << m_gangstersTable->getPlayerData(pid, "fightsMax")
             << player1.value("gold").toDouble() << ok;

    //qDebug() << "\t[dataParser::getResults]" << opponent.toVariantMap();
    //qDebug() << "\t[dataParser::getResults]" << fights.toVariantMap();
    if(happens.date().dayOfYear() == 1) {
        qDebug() << "\t[dataParser::getResults?]\n" << m_lastResult.toJson();
    }

    // for debugging only:
    //if(done >= 1) m_isActive = false;
}

// Tages-Aufgaben
void dataParser::diaryData(const QString diary)
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
                //qDebug() << "dataParser::diaryData\n" << debug.toJson();
            }
        }
    }
    //QJsonDocument debug(aufgaben);
    //qDebug() << "\t[dataParser::diaryData]\n" << debug.toJson();
}

void dataParser::getBattleEventData(const QString data)
{
    qDebug() << "dataParser::getBattleEventData" << data;
}

// Patenvilla Daten
void dataParser::patenvillaData(const QString villa)
{
    QByteArray data;
    data.append(villa);
    QJsonDocument json = QJsonDocument::fromJson(data);
    QJsonArray liste = json.object().value("list").toArray();
    for(QJsonArray::const_iterator i = liste.constBegin(); i != liste.end(); ++i) {
        QJsonObject feature = (*i).toObject();
        if(!feature.isEmpty() && feature.value("is_activate").toBool()) {
            QJsonDocument debug(feature);
            qDebug() << "\t[dataParser]::patenvillaData\n" << debug.toJson() << "\n";
        }
    }
    //QJsonDocument debug(liste);
    //qDebug() << "\t[dataParser]::patenvillaData\n" << debug.toJson() << "\n";
}

