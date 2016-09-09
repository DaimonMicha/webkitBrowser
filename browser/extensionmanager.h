#ifndef EXTENSIONMANAGER_H
#define EXTENSIONMANAGER_H

#include <QObject>
#include <QDir>
#include "extension.h"

class WebPage;

class ExtensionManager : public QObject
{
    Q_OBJECT
public:
    explicit ExtensionManager(QObject *parent = 0);

    void loadSettings();

signals:

public slots:
    void loadStarted(WebPage*,const QUrl &);
    void loadFinished(WebPage*);

    void loadFinished(QNetworkReply*);

private:
    QDir            pluginsDir;
    QList<ExtensionInterface *> m_extensionList;
};

#endif // EXTENSIONMANAGER_H
