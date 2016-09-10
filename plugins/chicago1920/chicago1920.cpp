#include "chicago1920.h"
#include "browserapplication.h"
#include "networkaccessmanager.h"

#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QWebFrame>
#include <QTime>

#include <QDebug>


Chicago1920::~Chicago1920()
{
    Q_CLEANUP_RESOURCE(data);
}

bool Chicago1920::isMyUrl(const QUrl &url) const
{
    QString host = url.host();
    if(host == QString("www.chicago1920.com")) return(true);
    return(false);
}

void Chicago1920::loadSettings(QSettings &settings)
{
    settings.beginGroup(QLatin1String("chicago1920"));
    settings.endGroup();
    Q_INIT_RESOURCE(data);
    // Create seed for the random
    // That is needed only once on application startup
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    qDebug() << "\tChicago1920::loadSettings";
}

void Chicago1920::loadStarted(WebPage* page,const QUrl &url)
{
    if(!isMyUrl(url)) return;
    QList<QNetworkCookie> cookies = page->networkAccessManager()->cookieJar()->cookiesForUrl(url);
    // ToDo: look for an account with this cookie
    qDebug() << "\tChicago1920::loadStarted" << url.toString()
             << cookies.count();
             //<< cookies.at(0).name() << cookies.at(0).value();
}

void Chicago1920::loadFinished(QNetworkReply* reply)
{
    QUrl url = reply->url();
    if(!isMyUrl(url)) return;

    QList<QNetworkCookie> cookies = reply->manager()->cookieJar()->cookiesForUrl(url);
    QByteArray cValue;
    if(cookies.count()) cValue = cookies.at(0).value();

    chAccount *current = NULL;
    if(m_accounts.count() > 0) foreach (chAccount *account, m_accounts) {
        if(account->cookieValue() == QString(cValue)) {
            current = account;
            break;
        }
    }
    if(current == NULL) {
        return;
    }

    current->loadFinished(reply);

    //return;

}

void Chicago1920::loadFinished(WebPage* page)
{
    QUrl url = page->mainFrame()->url();
    if(!isMyUrl(url)) return;
    QList<QNetworkCookie> cookies = page->networkAccessManager()->cookieJar()->cookiesForUrl(url);
    QByteArray cValue;
    if(cookies.count()) cValue = cookies.at(0).value();

    // ToDo: look for an account with this cookie
    chAccount *current = NULL;
    if(m_accounts.count() > 0) foreach (chAccount *account, m_accounts) {
        if(account->cookieValue() == QString(cValue)) {
            current = account;
            break;
        }
    }
    if(cValue.isEmpty()) return;

    if(current == NULL) {
        current = new chAccount(cValue);
        m_accounts.append(current);
    }

    QString path = url.path();

    page->mainFrame()->addToJavaScriptWindowObject("account", current);
    //qDebug() << "\tChicago1920::loadFinished" << path;
             //<< paths.at(0) << paths.at(1)
             //<< result
             //<< m_cookieValue;
    //return;
    current->loadFinished(page);
}
