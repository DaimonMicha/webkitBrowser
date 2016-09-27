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

    //connect(BrowserApplication::networkAccessManager(), SIGNAL(started()
    connect(BrowserApplication::networkAccessManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(loadFinished(QNetworkReply*)));

    foreach(QObject *plugin, QPluginLoader::staticInstances()) {
        ExtensionInterface *e = qobject_cast<ExtensionInterface *>(plugin);
        if(e) {
            qDebug() << "found static Extension:";// << fileName;
            m_extensionList.insert(e,"");
        }
    }
}

void ExtensionManager::loadPlugins()
{
    foreach (QString fileName, m_config.m_pluginsDir.entryList(QDir::Files)) {
        if(loadPlugin(m_config.m_pluginsDir.absoluteFilePath(fileName))) {
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
        ExtensionInterface* e = qobject_cast<ExtensionInterface *>(plugin);
        if(e) {
            m_extensionList.insert(e, filePath);
            ret = true;
            QStandardItem* item = new QStandardItem(e->name());
            item->setData(filePath,Qt::ToolTipRole);
            m_botsItem->appendRow(item);
            //connect(e, SIGNAL(logMessage(QString)), this, SLOT(log(QString)));
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

    m_config.m_dbConfig.m_driver = settings.value(QLatin1String("dbDriver"), "QMYSQL").toString();
    m_config.m_dbConfig.m_host = settings.value(QLatin1String("dbHost"), "localhost").toString();
    m_config.m_dbConfig.m_port = settings.value(QLatin1String("dbPort"), 3306).toInt();
    m_config.m_dbConfig.m_database = settings.value(QLatin1String("dbDatabase"), "ClickMaster").toString();
    m_config.m_dbConfig.m_user = settings.value(QLatin1String("dbUser"), "clickmaster").toString();
    m_config.m_dbConfig.m_password = settings.value(QLatin1String("dbPassword"), "retsamkcilc").toString();

    QStringList sqlDrivers = QSqlDatabase::drivers();
    if(sqlDrivers.contains(m_config.m_dbConfig.m_driver)) {
        QSqlDatabase db = QSqlDatabase::addDatabase(m_config.m_dbConfig.m_driver,QLatin1String("clickmasterConnection"));
        db.setHostName(m_config.m_dbConfig.m_host);
        db.setPort(m_config.m_dbConfig.m_port);
        db.setDatabaseName(m_config.m_dbConfig.m_database);
        db.setUserName(m_config.m_dbConfig.m_user);
        db.setPassword(m_config.m_dbConfig.m_password);
        bool ok = db.open();
    }

    m_config.m_pluginsDir.setPath(settings.value(QLatin1String("pluginsDir"), qApp->applicationDirPath() + "/plugins").toString());

    if(m_config.m_logFile.isOpen()) m_config.m_logFile.close();
    m_config.m_logFile.setFileName(settings.value(QLatin1String("logFile"), QStandardPaths::writableLocation(QStandardPaths::DataLocation)).toString());
    m_config.m_requestEnabled = settings.value(QLatin1String("requestEnabled"), false).toBool();
    m_config.m_responseEnabled = settings.value(QLatin1String("responseEnabled"), false).toBool();
    if(!m_config.m_logFile.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "...error while open the logfile" << m_config.m_logFile.fileName();
    }

    if(m_extensionList.count() == 0) loadPlugins();
    QMapIterator<ExtensionInterface *, QString> i(m_extensionList);
    while(i.hasNext()) {
        i.next();
        i.key()->loadSettings(settings);
    }

    settings.endGroup();
}

void ExtensionManager::saveToSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("extensions"));

    DatabaseDialog *database = qobject_cast<DatabaseDialog *>(settingsWidget("Database"));
    if(database) {
        settings.setValue(QLatin1String("dbDriver"), database->driversBox->currentText());
        settings.setValue(QLatin1String("dbHost"), database->hostEdit->text());
        settings.setValue(QLatin1String("dbPort"), database->portEdit->text());
        settings.setValue(QLatin1String("dbDatabase"), database->databaseEdit->text());
        settings.setValue(QLatin1String("dbUser"), database->userEdit->text());
        settings.setValue(QLatin1String("dbPassword"), database->passwordEdit->text());
    }

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
        dialog->logFileEdit->setText(m_config.m_logFile.fileName());
        dialog->checkRequests->setChecked(m_config.m_requestEnabled);
        dialog->checkResponses->setChecked(m_config.m_responseEnabled);
        ret = dialog;
    } else if(index.data().toString() == "Database") {
        DatabaseDialog *dialog = new DatabaseDialog();
        QStringList sqlDrivers = QSqlDatabase::drivers();
        if(sqlDrivers.contains(m_config.m_dbConfig.m_driver)) {
            dialog->driversBox->setCurrentText(m_config.m_dbConfig.m_driver);
        }
        dialog->hostEdit->setText(m_config.m_dbConfig.m_host);
        dialog->portEdit->setText(QString("%1").arg(m_config.m_dbConfig.m_port));
        dialog->databaseEdit->setText((m_config.m_dbConfig.m_database));
        dialog->userEdit->setText(m_config.m_dbConfig.m_user);
        dialog->passwordEdit->setText(m_config.m_dbConfig.m_password);
        //qDebug() << "ExtensionManager::settingsWidget" << sqlDrivers;
        ret = dialog;
    } else if(index.data().toString() == "ClickBots") {
        ClickBotsDialog *dialog = new ClickBotsDialog();
        dialog->pluginsDirEdit->setText(m_config.m_pluginsDir.path());
        dialog->clickbotsTree->setModel(m_extensionsModel);
        dialog->clickbotsTree->setRootIndex(m_extensionsModel->indexFromItem(m_botsItem));
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
    while(i.hasNext()) {
        i.next();
        if(i.key()->isMyUrl(url)) {
            if(m_config.m_requestEnabled) {
                log(QString("%1::loadStarted (%2)").arg(i.key()->name()).arg(url.toString()));
            }
            i.key()->loadStarted(page,url);
        }
    }
}

void ExtensionManager::loadFinished(QNetworkReply* reply)
{
    QUrl url = reply->url();
    QMapIterator<ExtensionInterface *, QString> i(m_extensionList);
    while(i.hasNext()) {
        i.next();
        if(i.key()->isMyUrl(url)) {
            if(m_config.m_responseEnabled) {
                log(QString("%1::replyFinished (%2)")
                    .arg(i.key()->name())
                    .arg(url.toString())
                    );
            }
            i.key()->loadFinished(reply);
        }
    }
}

void ExtensionManager::loadFinished(WebPage *page)
{
    QUrl url = page->mainFrame()->url();
    QMapIterator<ExtensionInterface *, QString> i(m_extensionList);
    while(i.hasNext()) {
        i.next();
        if(i.key()->isMyUrl(url)) {
            if(m_config.m_responseEnabled) {
                log(QString("%1::loadFinished (%2), %3 Bytes")
                    .arg(i.key()->name())
                    .arg(url.toString())
                    .arg(page->bytesReceived())
                    );
            }
            i.key()->loadFinished(page);
        }
    }
}

void ExtensionManager::log(const QString msg)
{
    if(m_config.m_logFile.isOpen()) {
        QDateTime now = QDateTime::currentDateTime();
        //logString.append(now.toString("[yyyy-MM-dd HH:mm:ss]"));
        QTextStream outStream(&m_config.m_logFile);
        outStream << now.toString("[yyyy-MM-dd HH:mm:ss] ");

        ExtensionInterface *iface = qobject_cast<ExtensionInterface*>(sender());
        if(iface != NULL) {
            outStream << iface->name() + " ";
        }

        outStream << msg << "\n";
        m_config.m_logFile.flush();
    }
}

void ExtensionManager::debug(const QString)
{
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
}



DatabaseDialog::DatabaseDialog(QScrollArea *parent)
    : QScrollArea(parent)
{
    setupUi(this);
    foreach(QString driver, QSqlDatabase::drivers()) {
        driversBox->addItem(driver);
    }
    connect(driversBox, SIGNAL(activated(QString)), this, SLOT(driverActivated(QString)));
}

void DatabaseDialog::driverActivated(const QString driver)
{
    qDebug() << "DatabaseDialog::driverActivated" << driver;
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
}
