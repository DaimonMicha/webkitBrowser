#include "infoworker.h"

#include <QWebFrame>
#include <QWebElement>
#include <QCryptographicHash>



infoWorker::infoWorker(QObject *parent) :
    QObject(parent),
    m_isActive(false),
    m_workingPage(new QWebPage)
{
    m_patenvillaSecret = "ghdh67TZGHb56fgsdfkk0";
    connect(m_workingPage, SIGNAL(loadFinished(bool)), this, SLOT(workFinished(bool)));
}

int infoWorker::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

QString infoWorker::pageTitle()
{
    QWebElement title = m_workingPage->mainFrame()->findFirstElement("title");
    return(title.toPlainText().trimmed());
}

void infoWorker::setOn()
{
    m_isActive = true;
    if(!pageTitle().contains("Kampfwartezeit:")) {
        //m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/fights/vip"));
        fightsStart();
    }
    qDebug() << "infoWorker::setOn" << pageTitle();
}

void infoWorker::setOff()
{
    if(!m_isActive) return;
    m_isActive = false;
    qDebug() << "infoWorker::setOff" << pageTitle();
}

// Vip-List
void infoWorker::enemysListJson(const QString result)
{
    emit(enemysList(result));
}

void infoWorker::patenvillaData(const QString result)
{
    emit(patenvilla(result));
}

void infoWorker::fightsVip()
{
    if(!m_isActive) return;
    m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/fights/vip"));
}

void infoWorker::fightsStart()
{
    if(!m_isActive) return;
    m_workingPage->mainFrame()->load(QUrl("http://www.chicago1920.com/fights"));
}

void infoWorker::fightsFinished()
{
    QWebFrame* mainFrame = m_workingPage->mainFrame();
    QUrl url = mainFrame->url();
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);

    if(paths.count() == 1) return;

    if(paths.at(1) == QString("vip")) {

        QString question = "new Ajax.Request('/fights/enemysListJson";
        question.append("',{asynchronous: false,method: 'POST',dataType: 'json',onSuccess: function(result){worker.enemysListJson(result.responseText);}});");
        qDebug() << question;
        //QVariant result = mainFrame->evaluateJavaScript(question);

    } else if(paths.at(1) == QString("start")) {

        if(paths.count() == 2) {
            emit(cooldownEnd());
        }

    } else if(paths.at(1) == QString("waitFight")) {

    }

    qDebug() << "\t[infoWorker::fightsFinished] (" + paths.at(1) + "):";
}

void infoWorker::workFinished(bool ok)
{
    if(!ok) return;

    QWebFrame* mainFrame = m_workingPage->mainFrame();
    QUrl url = mainFrame->url();
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);
    QVariant result;

    mainFrame->addToJavaScriptWindowObject("worker", this);

    if(paths.at(0) == QString("fights")) {

        fightsFinished();

    } else if(paths.at(0) == QString("patenvilla")) {
        if(paths.count() == 1) {
            QString question = "new Ajax.Request('/patenvilla/getData/";
            question.append(QCryptographicHash::hash(m_patenvillaSecret.toLatin1(), QCryptographicHash::Md5).toHex());
            question.append("',{asynchronous: false,method: 'POST',dataType: 'json',onSuccess: function(result){worker.patenvillaData(result.responseText);}});");
            //qDebug() << "results:" << question;
            //result = mainFrame->evaluateJavaScript(question);
            qDebug() << "infoWorker::workFinished (patenvilla): " << pageTitle();
        }
        //result = mainFrame->evaluateJavaScript("new Ajax.Request('/fights/opponentsListJson',{asynchronous: true,onSuccess: function(result){window.alert(result.responseText);}});");
    } else {

        qDebug() << "\t[infoWorker::workFinished] (" + paths.at(0) + ")" << paths;

    }
}
