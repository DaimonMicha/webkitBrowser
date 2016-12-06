#ifndef CHICAGO1920_H
#define CHICAGO1920_H

#include "extension.h"
//#include "webview.h"
#include "chaccount.h"

class Chicago1920  : public QObject, public ExtensionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "de.daimon-net.webkitBrowser.ExtensionInterface" FILE "chicago1920.json")
    Q_INTERFACES(ExtensionInterface)

public:
    ~Chicago1920();

    QString name() const { return(QLatin1String("Chicago1920")); }
    QWidget* settingsWidget() const;
    bool isMyUrl(const QUrl &) const;

    void loadSettings(QSettings &);
    void loadStarted(WebPage*,const QUrl &);
    void loadFinished(WebPage*);
    void dataReady(QNetworkReply*);
    void loadFinished(QNetworkReply*);

private:
    int readDataFile(const QString file, QString& data);
    void injectHtml(QWebFrame*, chAccount*);

private:
    QList<chAccount *>      m_accounts;
    QList<QPointer<WebPage> > m_webPages;
};

#endif // CHICAGO1920_H
