#include "bkaccount.h"

#include <QUrl>
#include <QDateTime>
#include <QWebFrame>

#include <QDebug>


bkAccount::bkAccount(const QString cookie, QObject *parent) :
    QObject(parent),
    m_cookieValue(cookie)
{
    m_config.bot = false;
}

void bkAccount::toggle(const QString option, const bool on)
{
    if(option == "account") {
        m_config.bot = on;
        if(on) {
        } else {
        }
    }
    //qDebug() << "bkAccount::toggle:" << option << on;
}

void bkAccount::loadFinished(QWebPage* page)
{
    QWebFrame* mainFrame = page->mainFrame();
    QUrl url = mainFrame->url();
    QString host = url.host();
    QStringList paths = url.path().split("/",QString::SkipEmptyParts);

    if(!paths.count()) return; // nothing to do, login evtl?

}
