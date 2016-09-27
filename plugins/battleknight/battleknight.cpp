#include "battleknight.h"

#include <QFile>
#include <QTime>
#include <QWebFrame>
#include <QWebElement>
#include <QNetworkCookie>
#include <QNetworkCookieJar>

#include <QDebug>


BattleKnight::~BattleKnight()
{
    Q_CLEANUP_RESOURCE(data);
}

bool BattleKnight::isMyUrl(const QUrl &url) const
{
    QString host = url.host();
    if(host.endsWith(".battleknight.gameforge.com")) return(true);
    return(false);
}

void BattleKnight::loadSettings(QSettings &settings)
{
    settings.beginGroup(QLatin1String("battleknight"));
    settings.endGroup();

    Q_INIT_RESOURCE(data);

    // Create seed for the random
    // That is needed only once on application startup
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    //qDebug() << "\tBattleKnight::loadSettings";
}

bkAccount *BattleKnight::accFromCookie(const QString cValue)
{
    bkAccount *ret = NULL;
    if(m_accounts.count() > 0) foreach (bkAccount *account, m_accounts) {
        if(account->cookieValue() == cValue) {
            ret = account;
            break;
        }
    }

    return(ret);
}

int BattleKnight::readDataFile(const QString file, QString& data)
{
    QString path;
    // path = "/home/micha/.local/share/DaimonNetworks/webkitBrowser";
    path = "/home/micha/Projekte/webkitBrowser/plugins/battleknight/htmls/";
    QFile inject;
    inject.setFileName(path + file);
    if(!inject.open(QIODevice::ReadOnly)) {
        inject.setFileName(":/battleknight/" + file);
        if(!inject.open(QIODevice::ReadOnly)) {
            return(-1);
        }
    }
    if(inject.isOpen()) {
        QByteArray bytes = inject.readAll();
        //qDebug() << "[BattleKnight::readDataFile]:" << inject.fileName();
        inject.close();
        data.append(bytes);
        return(data.length());
    }

    return(-1);
}

void BattleKnight::injectHtml(QWebFrame* mainFrame, bkAccount* account)
{
    QWebElement pluginDiv = mainFrame->findFirstElement("#accountPlugin");
    if(!pluginDiv.isNull()) return;

    QString di;
    if(readDataFile("inject.html", di) <= 0) return;

    QWebElement body = mainFrame->findFirstElement("body");
    body.appendInside(di);

    if(account->isActive("account")) {
        QWebElement checker = body.findFirst("#clickChecker");
        if(!checker.isNull()) checker.setAttribute("checked", "checked");
    }

    di.truncate(0);
    if(readDataFile("checkscript.js", di) <= 0) return;
    mainFrame->evaluateJavaScript(di);
}

void BattleKnight::loadStarted(WebPage*,const QUrl &url)
{
    if(!isMyUrl(url)) return;
    //qDebug() << "\tBattleKnight::loadStarted" << url.toString();
}

void BattleKnight::loadFinished(QNetworkReply*)
{
}

void BattleKnight::loadFinished(WebPage* page)
{
    QUrl url = page->mainFrame()->url();
    if(!isMyUrl(url)) return;

    QList<QNetworkCookie> cookies = page->networkAccessManager()->cookieJar()->cookiesForUrl(url);
    QByteArray cValue;
    if(!cookies.count()) return;
    cValue = cookies.at(0).value();
    if(cValue.isEmpty()) return;

    // look for an account with this cookie
    bkAccount *current = accFromCookie(QString(cValue));
    if(current == NULL) {
        current = new bkAccount(cValue);
        m_accounts.append(current);
    }

    page->mainFrame()->addToJavaScriptWindowObject("account", current);
    current->loadFinished(page);
    injectHtml(page->mainFrame(), current);

    QString logString;
    QDateTime now = QDateTime::currentDateTimeUtc();
    logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
    logString.append("  BattleKnight::loadFinished (" + url.path());
    logString.append(") '" + page->mainFrame()->title() + "'");
    //qDebug() << logString;
}
