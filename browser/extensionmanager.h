#ifndef EXTENSIONMANAGER_H
#define EXTENSIONMANAGER_H

#include <QObject>
#include <QStandardItemModel>
#include <QDir>
#include "extension.h"

class WebPage;

class ExtensionManager : public QObject
{
    Q_OBJECT
public:
    explicit ExtensionManager(QObject *parent = 0);

    void loadSettings();
    void saveToSettings();
    QStandardItemModel* settingsModel() const { return(m_extensionsModel); }
    QWidget* settingsWidget(const QModelIndex &index);

private:
    void loadPlugins();
    bool loadPlugin(const QString);
    QWidget* settingsWidget(const QString);

signals:

public slots:
    void loadStarted(WebPage*,const QUrl &);
    void loadFinished(WebPage*);
    void loadFinished(QNetworkReply*);

private:
    QDir                                pluginsDir;

    bool                                m_enabled;
    QDir                                m_pluginsDir;
    QFile                               m_logFile;
    bool                                m_requestEnabled;
    bool                                m_responseEnabled;

    QMap<ExtensionInterface *, QString> m_extensionList;

    QStandardItemModel*                 m_extensionsModel;
    QStandardItem*                      m_botsItem;
    QList<QWidget *>                    m_settingsDialogList;
};



#include "ui_ext_debug.h"

class DebugDialog : public QWidget, public Ui_Debug
{
    Q_OBJECT
public:
    DebugDialog(QWidget *parent = 0);
    ~DebugDialog();

private slots:
    void logFileChooser();
};


#include "ui_ext_database.h"

class DatabaseDialog : public QWidget, public Ui_Database
{
    Q_OBJECT
public:
    DatabaseDialog(QWidget *parent = 0);
};


#include "ui_ext_clickbots.h"

class ClickBotsDialog : public QWidget, public Ui_ClickBots
{
    Q_OBJECT
public:
    ClickBotsDialog(QWidget *parent = 0);

private slots:
    void pluginsDirChooser();
};

#endif // EXTENSIONMANAGER_H
