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

    m_minCooldown = 403;
    m_maxCooldown = 4267;

    m_currentRow = 0;
    m_currentColumn = 0;

    connect(m_workingPage->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObject()));
    connect(m_workingPage, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));
    connect(m_workingPage->mainFrame(), SIGNAL(titleChanged(QString)),
            this, SLOT(titleChanged(QString)));
    m_timerActive = false;
}

void traitorWorker::setTraitor(const int row,const int column)
{
    m_currentRow = row;
    m_currentColumn = column;
    int wTime = 240*1000 + randInt(m_minCooldown,m_maxCooldown);
    QTimer::singleShot(wTime, this, SLOT(startFight()));
    qDebug() << "traitorWorker::setTraitor" << row << column << wTime;
}

void traitorWorker::startFight()
{
    if(!m_isActive) return;
    if(pageTitle().contains("] Stadttournee Kampfwartezeit:")) {
        // ToDo: kwz errechnen und erneut einen Timer setzen.
        return;
    }
    if(m_currentColumn < 1 || m_currentRow < 1) return;
    QString path = QString("battleNpc/start/%1/%2").arg(m_currentColumn).arg(m_currentRow);
    QUrl url("http://www.chicago1920.com/"+path);
    m_workingPage->mainFrame()->load(url);
    m_timerActive = false;
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
        m_timerActive = false;
        //qDebug() << title << m_currentKWZ;
    } else if(title == "Chicago1920.com") {
        if(!m_timerActive) {
            m_timerActive = true;
            qDebug() << title << "timerActive!";
        }
    } else {
    }

}

QString traitorWorker::traitor(const QString & field)
{
    if(field == "") {
    }
}

void traitorWorker::getResults(const QString result)
{
    QByteArray data;
    data.append(result);
    QString pid;
    QJsonDocument json = QJsonDocument::fromJson(data);

    QJsonObject fights = json.object().value("fights").toObject();

    int done = fights.value("doneFights").toInt() + 1;
    int max = fights.value("maxFights").toInt();

    if(done > max) {
        // ToDo: choose next traitor
        if(m_currentColumn++ > 12) {
            m_currentColumn = 1;
            m_currentRow++;
        }
    }

    qDebug() << "traitorWorker::getResults" << m_currentRow << m_currentColumn;
    //qDebug() << "traitorWorker::getResults \n" << json.toJson();
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
                    //int wTime = 240*1000 + randInt(m_minCooldown,m_maxCooldown);
                    //QTimer::singleShot(wTime, this, SLOT(startFight()));
                }
            }
        }
    }

    QString logString;
    QDateTime now = QDateTime::currentDateTime();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append("  traitorWorker::loadFinished (" + mainFrame->url().path());
    logString.append(") <" + pageTitle() + ">");
    qDebug() << logString;

}
