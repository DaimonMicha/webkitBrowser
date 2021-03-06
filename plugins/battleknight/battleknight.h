#ifndef BATTLEKNIGHT_H
#define BATTLEKNIGHT_H

#include "extension.h"
#include "bkaccount.h"


class BattleKnight  : public QObject, public ExtensionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "de.daimon-net.webkitBrowser.ExtensionInterface" FILE "battleknight.json")
    Q_INTERFACES(ExtensionInterface)

public:
    ~BattleKnight();

    QString name() const { return(QLatin1String("BattleKnight")); }
    QWidget* settingsWidget() const { return(new QWidget()); }
    bool isMyUrl(const QUrl &) const;

    void loadSettings(QSettings &);
    void loadStarted(WebPage*,const QUrl &);
    void loadFinished(QNetworkReply*);
    void loadFinished(WebPage*);

private:
    bkAccount *accFromCookie(const QString);
    int readDataFile(const QString file, QString& data);
    void injectHtml(QWebFrame*, bkAccount*);

private:
    QList<bkAccount *>      m_accounts;
};

#endif // BATTLEKNIGHT_H
