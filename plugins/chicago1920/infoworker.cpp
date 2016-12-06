#include "infoworker.h"

#include <QDateTime>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>



infoWorker::infoWorker(QNetworkAccessManager* manager, QObject *parent) :
    QObject(parent),
    m_isActive(false),
    m_workingPage(new QWebPage),
    m_isLoading(false)
{
    m_patenvillaSecret = "ghdh67TZGHb56fgsdfkk0";
    m_workList << "fights/vip"
               << "placeOfHonour"
               //<< "patenvilla"
               //<< "battle"
               << "challenge/diary";

    m_minCooldown = 403;
    m_maxCooldown = 5267;

    setNetworkAccessManager(manager);

    m_workingPage->setForwardUnsupportedContent(false);

    connect(m_workingPage->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObject()));
    connect(m_workingPage->mainFrame(), SIGNAL(loadStarted()),
            this, SLOT(loadStarted()));
    connect(m_workingPage, SIGNAL(loadFinished(bool)),
            this, SLOT(workFinished(bool)));
}

QString infoWorker::gangster(const QString& field)
{
    if(field == "id") {
        return(m_gangsterData.gd_id);
    } else if(field == "coded_id") {
        return(m_gangsterData.gd_coded_id);
    } else if(field == "clan_coded") {
        return(m_gangsterData.gd_clan);
    } else if(field == "name") {
        return(m_gangsterData.gd_name);
    }
    return(QString());
}

// Vip-List
void infoWorker::enemysListJson(const QString result)
{
    // gangster-id auslesen
    QByteArray data;
    data.append(result);
    QJsonDocument json = QJsonDocument::fromJson(data);

    m_gangsterData.gd_id = json.object().value("char_id").toString();
}

void infoWorker::loadNextPage()
{
    if(!m_isActive) return;

    if(m_isLoading) {
        QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
        return;
    }

    if(m_workList.count() > 0) {
        QString url = m_workList.takeFirst();
        m_isLoading = true;
        m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/" + url));
        return;
    } else {
        if(m_gangsterData.gd_name.isEmpty()) placeOfHonour();
        else QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
    }
}

void infoWorker::fightsStart()
{
    if(m_workList.contains("fights")) return;
    m_workList.append("fights");
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
}

void infoWorker::fightsVip()
{
    if(m_workList.contains("fights/vip")) return;
    m_workList.append("fights/vip");
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
}

void infoWorker::placeOfHonour()
{
    characters();
    if(m_workList.contains("placeOfHonour")) return;
    m_workList.append("placeOfHonour");
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
}

void infoWorker::characters()
{
    if(m_workList.contains("characters")) return;
    m_workList.append("characters");
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
}

void infoWorker::saveMax()
{
    if(m_workList.contains("safe/maxeinzahlen")) return;
    m_workList.prepend("safe/maxeinzahlen");
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
}

void infoWorker::fightsFinished()
{
    QWebFrame* mainFrame = m_workingPage->mainFrame();
    //QUrl url = mainFrame->url();
    QStringList paths = mainFrame->url().path().split("/",QString::SkipEmptyParts);
    QVariant result;

    if(paths.count() == 1) return;

    if(paths.at(1) == QString("vip")) {

        QString question = "new Ajax.Request('/fights/enemysListJson";
        question.append("',{asynchronous: false,method: 'POST',dataType: 'json',onSuccess: function(result){worker.enemysListJson(result.responseText);}});");
        //qDebug() << question;
        result = mainFrame->evaluateJavaScript(question);

        //QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));

    } else if(paths.at(1) == QString("start")) {

        if(paths.count() == 2) {
            emit(cooldownEnd());
        }

    } else if(paths.at(1) == QString("waitFight")) {

    }
}

void infoWorker::gangsterStatus(const QString topic, const QString value)
{
    if(topic == "lifeCurrent") {
    } else if(topic == "lifeMax") {
        m_gangsterData.gd_max_life = value.toInt();
    } else if(topic == "fullLifeAfter") {
    } else if(topic == "currentCrystals") { // whisky
        m_gangsterData.gd_whisky = value.toInt();
    } else if(topic == "currentGold") { // dollar
        m_gangsterData.gd_dollar = value.toInt();
    } else if(topic == "currentStone") { // gangsterPunkte
        m_gangsterData.gd_gp = value.toInt();
    }
}

void infoWorker::workFinished(bool ok)
{
    m_isLoading = false;
    if(!ok) return;

    QWebFrame* mainFrame = m_workingPage->mainFrame();
    QUrl url = mainFrame->url();
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);
    QVariant result;

    if(!paths.count()) return;

    if(url.path() == "/signups/login") return;

    if(paths.at(0) == QString("fights")) {

        fightsFinished();

    } else if(paths.at(0) == QString("poh")) {

        if(paths.count() == 5) {
            if(paths.at(1) == QString("hof")) {
                if(paths.at(2) == QString("mwown")) {
                    if(paths.at(3) == QString("top")) {
                        if(paths.at(4) == QString("ruf")) {
                            m_gangsterData.gd_name = mainFrame->findFirstElement("a.link_me").toPlainText().trimmed();
                            QString link = mainFrame->findFirstElement("a.link_me").attribute("href");
                            m_gangsterData.gd_coded_id = link.split("/",QString::SkipEmptyParts).last();
                            if(!m_gangsterData.gd_coded_id.isEmpty()) m_workList.prepend("characters/profile/" + m_gangsterData.gd_coded_id);
                        }
                    }
                }
            }
        }

    } else if(paths.at(0) == QString("challenge")) {
        if(paths.count() == 2 && paths.at(1) == "diary") {
            //mainFrame->evaluateJavaScript("new Ajax.Request('/challenge/diary_data',{asynchronous: false,method: 'GET',dataType: 'json',onSuccess: function(result){worker.diaryData(result.responseText);}});");
        }
    } else if(paths.at(0) == QString("characters")) {
        if(paths.count() == 1) {
            m_gangsterData.gd_name = mainFrame->findFirstElement("div.char-mydata").toPlainText().trimmed();
            m_gangsterData.gd_level = mainFrame->findFirstElement("div.char-mydatal").toPlainText().trimmed();
        } else if(paths.count() == 3) {
            // is it this gangster?
            if(m_gangsterData.gd_coded_id == paths.at(2)) {
                QWebElementCollection anchors = mainFrame->findAllElements("a");
                foreach(QWebElement anchor, anchors) {
                    QString link = anchor.attribute("href");
                    if(link.startsWith("/alliances/profile")) {
                        m_gangsterData.gd_clan = link.split("/",QString::SkipEmptyParts).last();
                    }
                }
            }
        }
    } else if(paths.at(0) == QString("patenvilla")) {
    } else if(paths.at(0) == QString("battle")) {
    } else {
    }

    // read LifeBar variables
    QString question = "worker.gangsterStatus('currentCrystals', currentCrystals);\n";
    question.append("worker.gangsterStatus('currentGold', currentGold);\n");
    question.append("worker.gangsterStatus('currentStone', currentStone);\n");
    question.append("worker.gangsterStatus('lifeMax', lifeMax);\n");
    question.append("worker.gangsterStatus('lifeCurrent', lifeCurrent);\n");
    question.append("worker.gangsterStatus('fullLifeAfter', fullLifeAfter);\n");
    result = mainFrame->evaluateJavaScript(question);

    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));

    QString logString;
    QDateTime now = QDateTime::currentDateTime();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append(" infoWorker::loadFinished (" + url.path());
    logString.append(") '" + mainFrame->title() + "'");
    qDebug() << logString.toLocal8Bit().data() << m_workList;
}
