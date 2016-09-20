#include "extensionmanager.h"
#include "browserapplication.h"
#include "networkaccessmanager.h"
#include "webview.h"

#include <QtCore/QSettings>
#include <QStandardPaths>
#include <QFileDialog>
#include <QApplication>
#include <QWebFrame>
#include <QPluginLoader>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>

#include <QDebug>

ExtensionManager::ExtensionManager(QObject *parent) :
    QObject(parent),
    m_extensionsModel(new QStandardItemModel)
{
    QStringList head;
    head << QLatin1String("Topic");
    m_extensionsModel->setHorizontalHeaderLabels(head);
    m_extensionsModel->appendRow(new QStandardItem(QLatin1String("Database")));
    m_extensionsModel->appendRow(new QStandardItem(QLatin1String("Debug")));
    m_botsItem = new QStandardItem(QLatin1String("ClickBots"));
    m_extensionsModel->appendRow(m_botsItem);

    connect(BrowserApplication::networkAccessManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(loadFinished(QNetworkReply*)));

    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        ExtensionInterface *e = qobject_cast<ExtensionInterface *>(plugin);
        if(e) {
            qDebug() << "found static Extension:";// << fileName;
            m_extensionList.insert(e,"");
        }
    }

    return;

    m_pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (m_pluginsDir.dirName().toLower() == "debug" || m_pluginsDir.dirName().toLower() == "release")
        m_pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        m_pluginsDir.cdUp();
        m_pluginsDir.cdUp();
        m_pluginsDir.cdUp();
    }
#endif
    m_pluginsDir.cd("plugins");

    //qDebug() << "pluginsDir:" << pluginsDir.path();

    foreach (QString fileName, m_pluginsDir.entryList(QDir::Files)) {
        if(loadPlugin(m_pluginsDir.absoluteFilePath(fileName))) {
            //qDebug() << "\tload Extension:" << fileName;
        }
    }
}

void ExtensionManager::loadPlugins()
{
    foreach (QString fileName, m_pluginsDir.entryList(QDir::Files)) {
        if(loadPlugin(m_pluginsDir.absoluteFilePath(fileName))) {
            //qDebug() << "\tload Extension:" << fileName;
        }
    }
}

bool ExtensionManager::loadPlugin(const QString filePath)
{
    bool ret = false;

    if(!QLibrary::isLibrary(filePath)) return(ret);

    //qDebug() << "\ttest of dynamic Extension:" << fileName;
    QPluginLoader loader(filePath);
    QObject *plugin = loader.instance();
    if(plugin) {
        ExtensionInterface *e = qobject_cast<ExtensionInterface *>(plugin);
        if(e) {
            m_extensionList.insert(e, filePath);
            ret = true;
            m_botsItem->appendRow(new QStandardItem(e->name()));
        } else {
            qDebug() << "\t" << filePath << "is not an ExtensionInterface!";
        }
        //populateMenus(plugin);
    } else {
        qDebug() << "error loading:" << filePath << loader.errorString();
    }

    return(ret);
}

void ExtensionManager::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("extensions"));

    QStringList sqlDrivers = QSqlDatabase::drivers();
    qDebug() << "ExtensionManager::loadSettings" << sqlDrivers;

    m_pluginsDir.setPath(settings.value(QLatin1String("pluginsDir"), qApp->applicationDirPath() + "/plugins").toString());
    if(m_extensionList.count() == 0) loadPlugins();

    m_logFile.setFileName(settings.value(QLatin1String("logFile"), QStandardPaths::writableLocation(QStandardPaths::DataLocation)).toString()+"/webkitBrowser.log");
    m_requestEnabled = settings.value(QLatin1String("requestEnabled"), false).toBool();
    m_responseEnabled = settings.value(QLatin1String("responseEnabled"), false).toBool();

    QMapIterator<ExtensionInterface *, QString> i(m_extensionList);
    while (i.hasNext()) {
        i.next();
        i.key()->loadSettings(settings);
    }

    settings.endGroup();
}

void ExtensionManager::saveToSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("extensions"));

    DebugDialog *debug = qobject_cast<DebugDialog *>(settingsWidget("Debug"));
    if(debug) {
        settings.setValue(QLatin1String("logFile"), debug->logFileEdit->text());
        settings.setValue(QLatin1String("requestEnabled"), debug->checkRequests->isChecked());
        settings.setValue(QLatin1String("responseEnabled"), debug->checkResponses->isChecked());
    }

    ClickBotsDialog *clickBots = qobject_cast<ClickBotsDialog *>(settingsWidget("ClickBots"));
    if(clickBots) {
        settings.setValue(QLatin1String("pluginsDir"), clickBots->pluginsDirEdit->text());
    }

    settings.endGroup();
}

QWidget* ExtensionManager::settingsWidget(const QString name)
{
    QWidget *ret = NULL;

    for(int i = 0; i < m_settingsDialogList.size(); ++i) {
        if(m_settingsDialogList.at(i)->objectName() == name)
            return(m_settingsDialogList.at(i));
    }

    return(ret);
}

QWidget* ExtensionManager::settingsWidget(const QModelIndex &index)
{
    QWidget *ret = settingsWidget(index.data().toString());

    if(ret) return(ret);

    if(index.data().toString() == "Debug") {
        DebugDialog *dialog = new DebugDialog();
        dialog->logFileEdit->setText(m_logFile.fileName());
        dialog->checkRequests->setChecked(m_requestEnabled);
        dialog->checkResponses->setChecked(m_responseEnabled);
        ret = dialog;
    } else if(index.data().toString() == "Database") {
        DatabaseDialog *dialog = new DatabaseDialog();
        ret = dialog;
    } else if(index.data().toString() == "ClickBots") {
        ClickBotsDialog *dialog = new ClickBotsDialog();
        dialog->pluginsDirEdit->setText(m_pluginsDir.path());
        ret = dialog;
    } else if(index.parent().isValid()) {
        QStandardItem* test = m_extensionsModel->itemFromIndex(index.parent());
        if(test == m_botsItem) {
            qDebug() << "settingsWidget:" << index.data().toString() << ", parent:" << index.parent().data().toString();
        }
    }

    if(ret) m_settingsDialogList.append(ret);

    return(ret);
}

void ExtensionManager::loadStarted(WebPage *page,const QUrl &url)
{
    //qDebug() << "ExtensionManager::loadStarted" << url.toString();
    QMapIterator<ExtensionInterface *, QString> i(m_extensionList);
    while (i.hasNext()) {
        i.next();
        i.key()->loadStarted(page,url);
    }
}

void ExtensionManager::loadFinished(QNetworkReply* reply)
{
    QMapIterator<ExtensionInterface *, QString> i(m_extensionList);
    while (i.hasNext()) {
        i.next();
        i.key()->loadFinished(reply);
    }
}

void ExtensionManager::loadFinished(WebPage *page)
{
    //qDebug() << "ExtensionManager::loadFinished" << page->mainFrame()->url().toString();
    QMapIterator<ExtensionInterface *, QString> i(m_extensionList);
    while (i.hasNext()) {
        i.next();
        i.key()->loadFinished(page);
    }
}






DebugDialog::DebugDialog(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(logFileButton, SIGNAL(clicked()), this, SLOT(logFileChooser()));
}

DebugDialog::~DebugDialog()
{
    qDebug() << "~DebugDialog";
}

void DebugDialog::logFileChooser()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log-File"),
                               logFileEdit->text(),
                               tr("Log-Files (*.log *.txt)"));

    if(!fileName.isEmpty()) logFileEdit->setText(fileName);
    qDebug() << "DebugDialog::logFileChooser" << fileName;
}



DatabaseDialog::DatabaseDialog(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}



ClickBotsDialog::ClickBotsDialog(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(pluginsDirButton, SIGNAL(clicked()), this, SLOT(pluginsDirChooser()));
}

void ClickBotsDialog::pluginsDirChooser()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    pluginsDirEdit->text(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()) pluginsDirEdit->setText(dir);
    qDebug() << "ClickBotsDialog::pluginsDirChooser" << dir;
}
