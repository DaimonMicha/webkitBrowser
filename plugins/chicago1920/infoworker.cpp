#include "infoworker.h"

#include <QWebFrame>
#include <QWebElement>
#include <QTimer>
#include <QDateTime>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>



infoWorker::infoWorker(QObject *parent) :
    QObject(parent),
    m_isActive(false),
    m_workingPage(new QWebPage),
    m_isLoading(false)
{
    m_patenvillaSecret = "ghdh67TZGHb56fgsdfkk0";
    m_workList << "fights/vip" << "placeOfHonour" << "challenge/diary";// << "patenvilla";

    m_minCooldown = 403;
    m_maxCooldown = 5267;

    connect(m_workingPage->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObject()));
    connect(m_workingPage->mainFrame(), SIGNAL(titleChanged(QString)),
            this, SLOT(titleChanged(QString)));
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
    }
    return(QString());
}

void infoWorker::loadStarted()
{
    m_isLoading = true;
}

void infoWorker::titleChanged(const QString &)
{
    //qDebug() << "infoWorker::titleChanged" << title;
}

int infoWorker::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

QString infoWorker::pageTitle()
{
    QWebElement title = m_workingPage->mainFrame()->findFirstElement("title");
    //if(title.isNull()) return(QString());
    return(title.toPlainText().trimmed());
}

void infoWorker::addJavaScriptObject()
{
    m_workingPage->mainFrame()->addToJavaScriptWindowObject("worker", this);
}

void infoWorker::setOn()
{
    if(m_isActive) return;

    m_isActive = true;
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
    //qDebug() << "infoWorker::setOn" << pageTitle();
}

void infoWorker::setOff()
{
    if(!m_isActive) return;
    m_isActive = false;
    //qDebug() << "infoWorker::setOff" << pageTitle();
}

// diary data
void infoWorker::diaryData(const QString result)
{
    emit(diarydata(result));
}

// Vip-List
void infoWorker::enemysListJson(const QString result)
{
    // ToDo: gangster-id auslesen
    QByteArray data;
    data.append(result);
    QJsonDocument json = QJsonDocument::fromJson(data);
    m_gangsterData.gd_id = json.object().value("char_id").toString();
    //qDebug() << "infoWorker::enemysListJson" << json.object().value("char_id");

    emit(enemysList(result));
}

// Patenvilla Daten
void infoWorker::patenvillaData(const QString result)
{
    emit(patenvilla(result));
}

void infoWorker::loadNextPage()
{
    if(!m_isActive) return;

    if(m_isLoading) QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));

    if(m_workList.count() > 0) {
        QString url = m_workList.takeFirst();
        m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/" + url));
        return;
    } else {
        QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
    }
}

void infoWorker::fightsStart()
{
    if(!m_isActive) return;

    m_workList.prepend("fights");
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
}

void infoWorker::fightsVip()
{
    if(!m_isActive) return;

    m_workList.prepend("fights/vip");
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
}

void infoWorker::placeOfHonour()
{
    if(!m_isActive) return;

    m_workList.prepend("placeOfHonour");
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
}

void infoWorker::characters()
{
    if(!m_isActive) return;

    m_workList.prepend("characters");
    QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
}

void infoWorker::fightsFinished()
{
    QWebFrame* mainFrame = m_workingPage->mainFrame();
    QUrl url = mainFrame->url();
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);
    QVariant result;

    if(paths.count() == 1) return;

    if(paths.at(1) == QString("vip")) {

        QString question = "new Ajax.Request('/fights/enemysListJson";
        question.append("',{asynchronous: false,method: 'POST',dataType: 'json',onSuccess: function(result){worker.enemysListJson(result.responseText);}});");
        //qDebug() << question;
        result = mainFrame->evaluateJavaScript(question);

        QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));

    } else if(paths.at(1) == QString("start")) {

        if(paths.count() == 2) {
            emit(cooldownEnd());
        }

    } else if(paths.at(1) == QString("waitFight")) {

    }

    //if(!result.isNull()) qDebug() << "\t[infoWorker::fightsFinished]\n" << result.toMap().value("constructor").toMap().keys(); //.keys() auf "list" achten...
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

    if(paths.at(0) == QString("fights")) {

        fightsFinished();

    } else if(paths.at(0) == QString("poh")) {

        if(paths.count() == 5) {
            if(paths.at(1) == QString("hof")) {
                if(paths.at(2) == QString("mwown")) {
                    if(paths.at(3) == QString("top")) {
                        if(paths.at(4) == QString("ruf")) {
                            //qDebug() << mainFrame->findFirstElement("a.link_me").toPlainText().trimmed();
                            m_gangsterData.gd_name = mainFrame->findFirstElement("a.link_me").toPlainText().trimmed();
                            QString link = mainFrame->findFirstElement("a.link_me").attribute("href");
                            m_gangsterData.gd_coded_id = link.split("/",QString::SkipEmptyParts).last();
                            m_workList.prepend("characters/profile/" + m_gangsterData.gd_coded_id);
                            //qDebug() << m_gangsterData.gd_coded_id;
                            QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));
                        }
                    }
                }
            }
        }

    } else if(paths.at(0) == QString("challenge")) {
        if(paths.count() == 2 && paths.at(1) == "diary") {
            mainFrame->evaluateJavaScript("new Ajax.Request('/challenge/diary_data',{asynchronous: false,method: 'GET',dataType: 'json',onSuccess: function(result){worker.diaryData(result.responseText);}});");
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
                        //qDebug() << "infoWorker - player.clan_id" << m_gangsterData.gd_clan;
                    }
                }
            }
        }

        QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(loadNextPage()));

    } else if(paths.at(0) == QString("patenvilla")) {
        if(paths.count() == 1) {
            QString question = "new Ajax.Request('/patenvilla/getData/";
            question.append(QCryptographicHash::hash(m_patenvillaSecret.toLatin1(), QCryptographicHash::Md5).toHex());
            question.append("/" + m_gangsterData.gd_clan);
            question.append("',{asynchronous: false,method: 'POST',dataType: 'json',onSuccess: function(result){worker.patenvillaData(result.responseText);}});");
            //qDebug() << "results:" << question;
            if(!m_gangsterData.gd_clan.isEmpty()) result = mainFrame->evaluateJavaScript(question);
            //qDebug() << "infoWorker::workFinished (patenvilla): " << pageTitle();
        }
        //result = mainFrame->evaluateJavaScript("new Ajax.Request('/fights/opponentsListJson',{asynchronous: true,onSuccess: function(result){window.alert(result.responseText);}});");
    } else {

    }

    QString logString;
    QDateTime now = QDateTime::currentDateTimeUtc();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append("  infoWorker::loadFinished (" + url.path());
    logString.append(") '" + mainFrame->title() + "'");
    qDebug() << logString;
    //qDebug() << pageTitle() << "\t[infoWorker::workFinished]" << paths;
}
