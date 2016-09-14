#include "fightworker.h"

#include <QWebFrame>
#include <QWebElement>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QDateTime>

#include <QDebug>

fightWorker::fightWorker(QObject *parent) :
    QObject(parent),
    m_isActive(false),
    m_workingPage(new QWebPage),
    m_currentFightCounter(0)
{
    m_currentOpponent = "";
    connect(m_workingPage, SIGNAL(loadFinished(bool)), this, SLOT(workFinished(bool)));
}

void fightWorker::setOn()
{
    m_isActive = true;
    if(m_workingPage->mainFrame()->url().path() != "/fights/waitFight") {
        m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/fights"));
    }
    //qDebug() << "fightWorker::setOn" << m_workingPage->mainFrame()->url().path() << pageTitle();
}

void fightWorker::setOff()
{
    if(!m_isActive) return;
    m_isActive = false;
    //m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/fights"));
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
    if(m_isActive) {
        if(!pageTitle().contains("Kampfwartezeit:")) m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/fights"));
    }
    //qDebug() << "fightWorker::setOpponent:" << m_currentOpponent;
}

void fightWorker::startFight()
{
    if(!m_isActive) return;
    if(m_currentOpponent == "") return;
    QUrl url("http://www.chicago1920.com/fights/start/" + m_currentOpponent);
    // /fights/start_police/
    m_workingPage->mainFrame()->load(url);
    //qDebug() << "fightWorker::startFight" << url;
}

void fightWorker::waitFight()
{
    if(!m_isActive) return;
    if(m_currentOpponent == "") return;
    m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/fights"));
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

    QByteArray data;
    data.append(result);
    QJsonDocument json = QJsonDocument::fromJson(data);

    QJsonObject fights = json.object().value("fights").toObject();

    int done = fights.value("doneFights").toInt() + 1;
    int max = fights.value("maxFights").toInt();

    if(done >= max) {
        //m_currentOpponent.clear();
        //emit(fightsDone());
    }
    //emit(fightReady(done, max));

    // for debugging only:
    //if(done >= 1) m_isActive = false;
    //qDebug() << "fightWorker::getResults\n" << fights.toVariantMap();
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

    mainFrame->addToJavaScriptWindowObject("fighter", this);

    if(QString("fights") == paths.at(0)) {

        if(paths.count() > 1) {

            if(QString("results") == paths.at(1)) {

                if(paths.count() > 2) {
                    // ToDo:
                    // evaluate the cooldown-time and send a signal.
                    qDebug() << "fightWorker::workFinished (results): " << pageTitle();
                    QString question = "new Ajax.Request('/fights/getResults/";
                    question.append(paths.at(2));
                    question.append("',{asynchronous: false,method: 'GET',dataType: 'json',onSuccess: function(result){fighter.getResults(result.responseText);}});");
                    result = mainFrame->evaluateJavaScript(question);


                    QTimer::singleShot(randInt(266,23500), this, SLOT(waitFight()));
                    m_currentFightCounter = 0;
                }

            } else if(QString("waitFight") == paths.at(1)) {

                //qDebug() << "fightWorker::workFinished (waitFight): " << pageTitle();

            } else if(QString("waitLp") == paths.at(1)) {

                //if(m_isActive) QTimer::singleShot(randInt(1234,9876), this, SLOT(waitFight()));
                //qDebug() << "fightWorker::workFinished (waitLp): " << pageTitle();

            } else if(QString("start") == paths.at(1)) {

                if(paths.count() == 2) {
                    if(++m_currentFightCounter > 1) {
                        emit(fightsDone());
                        m_currentFightCounter = 0;
                    }
                    if(m_isActive) QTimer::singleShot(randInt(123,2857), this, SLOT(startFight()));
                }

            } else if(QString("fight") == paths.at(1)) {
                //TrefferZonen
                QString question = "new Ajax.Request('/fights/fightData";
                question.append("',{asynchronous: false,method: 'GET',dataType: 'json',onSuccess: function(result){fighter.fightData(result.responseText);}});");
                result = mainFrame->evaluateJavaScript(question);
                //qDebug() << "fightWorker::workFinished (fight): " << pageTitle();
            }

        }

    }

    QString logString;
    QDateTime now = QDateTime::currentDateTimeUtc();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append("  fightWorker::loadFinished " + url.path());
    logString.append(" '" + mainFrame->title() + "'");
    qDebug() << logString;
    //qDebug() << pageTitle() << "\t[fightWorker::loadFinished]" << paths << mainFrame->requestedUrl().path();
}
