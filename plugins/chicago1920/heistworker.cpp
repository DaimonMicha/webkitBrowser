#include "heistworker.h"

#include <QDateTime>
#include <QWebFrame>

#include <QDebug>


heistWorker::heistWorker(QObject *parent) :
    QObject(parent),
    m_isActive(false),
    m_workingPage(new QWebPage)
{
    connect(m_workingPage->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObject()));
    connect(m_workingPage, SIGNAL(loadFinished(bool)),
            this, SLOT(workFinished(bool)));
}

void heistWorker::addJavaScriptObject()
{
    m_workingPage->mainFrame()->addToJavaScriptWindowObject("heist", this);
}

void heistWorker::getResults(const QString data)
{
    qDebug() << "heistWorker::getResults" << data;
}

void heistWorker::workFinished(bool ok)
{
    if(!ok) return;

    QWebFrame* mainFrame = m_workingPage->mainFrame();
    QUrl url = mainFrame->url();
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);
    QVariant result;

    if(!paths.count()) return;

    if(paths.at(0) == QString("fights")) {

        // parseFights(m_workingPage, paths);

    } else if(QString("special") == paths.at(0)) {

        if(paths.count() > 1) {

            if(QString("heist") == paths.at(1)) {

                // PageTitle auslesen wegen Kampfwartezeit

            }

        }

    } else if(QString("battleNpc") == paths.at(0)) {

        // "/battleNpc/start/333/1"
        if(paths.count() > 1) {

            if(QString("results") == paths.at(1)) {

                if(paths.count() > 2) {

                    QString question = "new Ajax.Request('/battleNpc/getResults/";
                    question.append(paths.at(2));
                    question.append("',{asynchronous: false,method: 'GET',dataType: 'json',onSuccess: function(result){heist.getResults(result.responseText);}});");
                    result = mainFrame->evaluateJavaScript(question);
                    // ToDo: 5Minuten-Timer stellen
                    //QTimer::singleShot(301500, this, SLOT(heistWork()));
                    qDebug() << "results:" << question;

                }

            }

        }

    }

    QString logString;
    QDateTime now = QDateTime::currentDateTimeUtc();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append("  heistWorker::loadFinished (" + url.path());
    logString.append(") '" + mainFrame->title() + "'");
    qDebug() << logString;

}
