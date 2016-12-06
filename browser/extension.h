#ifndef EXTENSION_H
#define EXTENSION_H

#include <QtPlugin>
#include <QSettings>
#include <QUrl>
#include <QNetworkReply>
#include <QWidget>
#include "webview.h"

class ExtensionInterface
{
public:
    virtual ~ExtensionInterface() {}

    virtual QString name() const { return(QLatin1String("ExtensionInterface")); }
    virtual QWidget* settingsWidget() const { return(new QWidget()); }
    virtual bool isMyUrl(const QUrl &) const { return(false); }

    virtual void loadSettings(QSettings &) {}
    virtual void loadStarted(WebPage*,const QUrl &) {}
    virtual void dataReady(QNetworkReply*) {}
    virtual void loadFinished(QNetworkReply*) {}
    virtual void loadFinished(WebPage*) {}
};

#define ExtensionInterface_iid "de.daimon-net.webkitBrowser.ExtensionInterface"
Q_DECLARE_INTERFACE(ExtensionInterface, ExtensionInterface_iid)

#endif // EXTENSION_H
