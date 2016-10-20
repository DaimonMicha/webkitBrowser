#include "traitorworker.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

traitorWorker::traitorWorker(QObject *parent) :
    QObject(parent),
    m_isActive(false),
    m_workingPage(new QWebPage)
{
    m_currentKWZ = 0;

    m_minCooldown = 504;
    m_maxCooldown = 4267;

    m_currentRow = 0;
    m_currentColumn = 0;

    m_traitor.fightsDone = 0;
    m_traitor.fightsMax = 20;

    m_currentFightCounter = 0;
    m_fightTimer = 0;

    connect(m_workingPage->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObject()));
    connect(m_workingPage, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));
    connect(m_workingPage->mainFrame(), SIGNAL(titleChanged(QString)),
            this, SLOT(titleChanged(QString)));
}

void traitorWorker::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_fightTimer) {
        killTimer(m_fightTimer);
        m_fightTimer = 0;
        startFight();
    }
}

void traitorWorker::setTraitor(const int row,const int column)
{
    m_currentRow = row;
    m_currentColumn = column;
    int wTime = 240*1000 + randInt(m_minCooldown,m_maxCooldown);
    //QTimer::singleShot(wTime, this, SLOT(startFight()));
    if(m_fightTimer != 0) {
        killTimer(m_fightTimer);
        m_fightTimer = 0;
    }
    m_fightTimer = startTimer(wTime);
    qDebug() << "traitorWorker::setTraitor" << row << column << wTime;
}

void traitorWorker::startFight()
{
    if(!m_isActive) return;
    if(m_currentColumn < 1 || m_currentRow < 1) return;
    if(m_fightTimer) {
        killTimer(m_fightTimer);
        m_fightTimer = 0;
    }
    if(pageTitle().contains("] Stadttournee Kampfwartezeit:")) {
        // ToDo: kwz errechnen und erneut einen Timer setzen.
        //QTimer::singleShot(randInt(m_minCooldown,m_maxCooldown), this, SLOT(startFight()));
        m_fightTimer = startTimer(randInt(m_minCooldown,m_maxCooldown));
        return;
    }
    QString path = QString("battleNpc/start/%1/%2").arg(m_currentColumn).arg(m_currentRow);
    QUrl url("http://www.chicago1920.com/"+path);
    m_workingPage->mainFrame()->load(url);

    qDebug() << "traitorWorker::startFight" << url.toString();
}

void traitorWorker::titleChanged(const QString& title)
{
    if(title.contains("] Stadttournee Kampfwartezeit:")) {
        QString ts = title.split("] ").first();
        ts.remove(0,1);
        QTime kwz = QTime::fromString(ts, "hh:mm:ss");
        QTime mid(0,0,0,0);
        m_currentKWZ = mid.secsTo(kwz);
    } else if(title == "Chicago1920.com") {
        m_currentKWZ = 0;
    } else {
    }

}

QString traitorWorker::traitor(const QString& field)
{
    QString ret;

    if(field == "fightsMax") {
        ret = QString("%1").arg(m_traitor.fightsMax);
    } else if(field == "fightsDone") {
        ret = QString("%1").arg(m_traitor.fightsDone);
    }

    return(ret);
}

void traitorWorker::getResults(const QString result)
{
    QByteArray data;
    data.append(result);
    QString pid;
    QJsonDocument json = QJsonDocument::fromJson(data);

    QJsonObject fights = json.object().value("fights").toObject();

    m_traitor.fightsDone = fights.value("doneFights").toInt(); // + 1
    m_traitor.fightsMax = fights.value("maxFights").toInt();

    if(m_traitor.fightsDone >= m_traitor.fightsMax) {
        // choose next traitor
        m_currentColumn++;
    }

    qDebug() << "traitorWorker::getResults" << m_traitor.fightsDone << m_traitor.fightsMax << m_currentColumn;
    //qDebug() << "traitorWorker::getResults \n" << json.toJson();
}

void traitorWorker::midnightReset()
{
    if(m_currentColumn > 0) m_currentColumn = 1;
    if(m_currentRow > 0) m_currentRow = 1;
}

void traitorWorker::loadFinished(bool ok)
{
    if(!ok) return;

    QWebFrame* mainFrame = m_workingPage->mainFrame();
    QStringList paths = mainFrame->url().path().split("/",QString::SkipEmptyParts);
    QVariant result;

    if(!paths.count()) return;

    if(paths.at(0) == QString("battleNpc")) {
        if(paths.count() > 1) {
            if(QString("results") == paths.at(1)) {
                if(paths.count() == 3) {
                    QString question = "new Ajax.Request('/battleNpc/getResults/";
                    question.append(paths.at(2));
                    question.append("',{asynchronous: false,method: 'GET',dataType: 'json',onSuccess: function(result){worker.getResults(result.responseText);}});");
                    result = mainFrame->evaluateJavaScript(question);
                    m_currentFightCounter = 0;
                }
            }
        }
    } else if(paths.at(0) == QString("battle")) {
        if(paths.count() > 1) {
            if(QString("index") == paths.at(1)) {
                // ToDo: test another traitor
                if(++m_currentFightCounter > 2) {
                    m_currentColumn++;
                    m_currentFightCounter = 0;
                    startFight();
                    //setTraitor(m_currentRow, m_currentColumn);
                }
            }
        }
    }

    QString logString;
    QDateTime now = QDateTime::currentDateTime();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append(" traitorWorker::loadFinished (" + mainFrame->url().path());
    logString.append(") <" + pageTitle() + ">");
    qDebug() << logString.toLocal8Bit().data();

}
