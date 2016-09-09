#ifndef CHICAGO1920_H
#define CHICAGO1920_H

#include "extension.h"
//#include "webview.h"
#include "chaccount.h"

class Chicago1920 : public QObject, public ExtensionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "de.daimon-net.gamebrowser.ExtensionInterface" FILE "chicago1920.json")
    Q_INTERFACES(ExtensionInterface)

public:
    ~Chicago1920();

    bool isMyUrl(const QUrl &) const;

    void loadSettings(QSettings &);
    void loadStarted(WebPage*,const QUrl &);
    void loadFinished(WebPage*);
    void loadFinished(QNetworkReply*);

private:
    QList<chAccount *>      m_accounts;
};

#endif // CHICAGO1920_H
