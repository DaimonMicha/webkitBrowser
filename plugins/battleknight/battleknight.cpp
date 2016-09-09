#include "battleknight.h"

#include <QWebFrame>

#include <QDebug>


BattleKnight::~BattleKnight()
{
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
    //qDebug() << "\tBattleKnight::loadSettings";
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
    if(!isMyUrl(page->mainFrame()->url())) return;
    //qDebug() << "\tBattleKnight::loadFinished" << page->mainFrame()->url().toString();
}
