#include "fightworker.h"

#include <QWebFrame>
#include <QWebElement>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QDateTime>
#include <QTime>

#include <QDebug>

fightWorker::fightWorker(QObject *parent) :
    QObject(parent),
    m_isActive(false),
    m_workingPage(new QWebPage),
    m_currentFightCounter(0)
{
    m_currentOpponent = "";
    connect(m_workingPage->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObject()));
    connect(m_workingPage, SIGNAL(loadFinished(bool)),
            this, SLOT(workFinished(bool)));
}

void fightWorker::setOn()
{
    m_isActive = true;
    waitFight();
    //qDebug() << "fightWorker::setOn" << m_workingPage->mainFrame()->url().path() << pageTitle();
}

void fightWorker::setOff()
{
    if(!m_isActive) return;
    m_isActive = false;
    //qDebug() << "fightWorker::setOff" << pageTitle();
}

void fightWorker::setOpponent(const QString opponent)
{
    if(m_currentOpponent != opponent) {
        m_currentOpponent = opponent;
    }
    if(m_currentOpponent == "") {
        m_isActive = false;
    }
    waitFight();
    //qDebug() << "fightWorker::setOpponent:" << m_currentOpponent;
}

void fightWorker::setRival(const QString rival)
{
    if(m_currentRival != rival) {
        m_currentRival = rival;
    }
}

void fightWorker::waitFight()
{
    if(!m_isActive) return;
    if(m_currentOpponent == "") return;
    if(m_workingPage->mainFrame()->url().path() == "/fights/waitFight") return;
    m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/fights"));
}

void fightWorker::startFight()
{
    if(!m_isActive) return;
    if(m_currentOpponent == "") return;
    QUrl url("http://www.chicago1920.com/fights/start/" + m_currentOpponent);
    if(!m_currentRival.isEmpty()) {
        url.setUrl("http://www.chicago1920.com/rivalNpc/start/" + m_currentRival);
        m_currentRival = "";
    }
    m_workingPage->mainFrame()->load(url);
    //qDebug() << "fightWorker::startFight" << url;
    // http://www.chicago1920.com/rivalNpc/start/484781
    // http://www.chicago1920.com/rivalNpc/fight
    // http://www.chicago1920.com/rivalNpc/results/6205f39177b65b9d12cd93a16c06390f
    // http://www.chicago1920.com/rivalNpc/getResults/6205f39177b65b9d12cd93a16c06390f
}

void fightWorker::fightData(const QString result)
{
    /*
    QByteArray data;
    data.append(result);
    QJsonDocument json = QJsonDocument::fromJson(data);
*/
    //qDebug() << "fightWorker::fightData\n" << json.toJson();
    emit(fightDataReady(result));
}

void fightWorker::getResults(const QString result)
{
    emit(fightResults(result));
}

void fightWorker::addJavaScriptObject()
{
    m_workingPage->mainFrame()->addToJavaScriptWindowObject("fighter", this);
}

int fightWorker::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

QString fightWorker::pageTitle()
{
    QWebElement title = m_workingPage->mainFrame()->findFirstElement("title");
    return(title.toPlainText().trimmed());
}

void fightWorker::workFinished(bool ok)
{
    if(!ok) return;

    QWebFrame* mainFrame = m_workingPage->mainFrame();
    QUrl url = mainFrame->url();
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);
    QVariant result;

    if(QString("fights") == paths.at(0) || QString("rivalNpc") == paths.at(0)) {

        if(paths.count() > 1) {

            if(QString("results") == paths.at(1)) {

                if(paths.count() == 3) {
                    // evaluate the cooldown-time and send a signal.
                    QString title = pageTitle();
                    if(title.contains("] Kampfwartezeit:")) {
                        title = title.split("] ").first();
                        title.remove(0,1);
                        QTime kwz = QTime::fromString(title, "hh:mm:ss");
                        QTime mid(0,0,0,0);
                        m_currentKWZ = mid.secsTo(kwz);
                        emit(fightCooldown(m_currentKWZ));
                    }

                    QString question = "new Ajax.Request('/"+paths.at(0)+"/getResults/";
                    question.append(paths.at(2));
                    question.append("',{asynchronous: false,method: 'GET',dataType: 'json',onSuccess: function(result){fighter.getResults(result.responseText);}});");
                    result = mainFrame->evaluateJavaScript(question);

                    QTimer::singleShot(randInt(266,23500), this, SLOT(waitFight()));
                    m_currentFightCounter = 0;
                }

            } else if(QString("waitFight") == paths.at(1)) {

                //qDebug() << "fightWorker::workFinished (waitFight): " << pageTitle();

            } else if(QString("waitLp") == paths.at(1)) {

                //QTimer::singleShot(randInt(3456,9876), this, SLOT(waitFight()));
                qDebug() << "fightWorker::workFinished (waitLp): " << pageTitle();

            } else if(QString("start") == paths.at(1)) {

                if(paths.count() == 2) {
                    if(++m_currentFightCounter > 2) {
                        emit(fightsDone());
                        m_currentFightCounter = 0;
                    }
                    QTimer::singleShot(randInt(1234,3857), this, SLOT(startFight()));
                }

            } else if(QString("fight") == paths.at(1)) {
                //TrefferZonen
                QString question = "new Ajax.Request('/"+paths.at(0)+"/fightData";
                question.append("',{asynchronous: false,method: 'GET',dataType: 'json',onSuccess: function(result){fighter.fightData(result.responseText);}});");
                result = mainFrame->evaluateJavaScript(question);
                //qDebug() << "fightWorker::workFinished (fight): " << pageTitle();
            }

        }

    }

    QString logString;
    QDateTime now = QDateTime::currentDateTime();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append("  fightWorker::loadFinished (" + url.path());
    logString.append(") '" + mainFrame->title() + "'");
    qDebug() << logString;
}
