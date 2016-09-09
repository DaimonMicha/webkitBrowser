#include "extensionmanager.h"
#include "browserapplication.h"
#include "networkaccessmanager.h"
#include "webview.h"

#include <QApplication>
#include <QWebFrame>
#include <QPluginLoader>
#include <QDebug>

ExtensionManager::ExtensionManager(QObject *parent) :
    QObject(parent)
{
    //NetworkAccessManager* manager = BrowserApplication::networkAccessManager();
    connect(BrowserApplication::networkAccessManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(loadFinished(QNetworkReply*)));

    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        ExtensionInterface *e = qobject_cast<ExtensionInterface *>(plugin);
        if(e) {
            qDebug() << "found static Extension:";// << fileName;
            m_extensionList.append(e);
        }
    }

    pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");

    //qDebug() << "pluginsDir:" << pluginsDir.path();

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        if (QLibrary::isLibrary(pluginsDir.absoluteFilePath(fileName))) {
            //qDebug() << "\ttest of dynamic Extension:" << fileName;
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin) {
                //qDebug() << "\tfound Extension:" << fileName;
                ExtensionInterface *e = qobject_cast<ExtensionInterface *>(plugin);
                if(e) {
                    m_extensionList.append(e);
                } else {
                    qDebug() << "\t" << fileName << "is not an ExtensionInterface!";
                }
                //populateMenus(plugin);
            } else {
                qDebug() << "error loading:" << fileName << loader.errorString();
            }
        }
    }
}

void ExtensionManager::loadSettings()
{
    //qDebug() << "ExtensionManager::loadSettings" << m_extensionList.count();
    QSettings settings;
    settings.beginGroup(QLatin1String("extensions"));

    foreach (ExtensionInterface *plugin, m_extensionList) {
        plugin->loadSettings(settings);
    }
    settings.endGroup();
}

void ExtensionManager::loadStarted(WebPage *page,const QUrl &url)
{
    //qDebug() << "ExtensionManager::loadStarted" << url.toString();
    foreach (ExtensionInterface *plugin, m_extensionList) {
        plugin->loadStarted(page,url);
    }
}

void ExtensionManager::loadFinished(QNetworkReply* reply)
{
    foreach (ExtensionInterface *plugin, m_extensionList) {
        plugin->loadFinished(reply);
    }
}

void ExtensionManager::loadFinished(WebPage *page)
{
    //qDebug() << "ExtensionManager::loadFinished" << page->mainFrame()->url().toString();
    foreach (ExtensionInterface *plugin, m_extensionList) {
        plugin->loadFinished(page);
    }
}
