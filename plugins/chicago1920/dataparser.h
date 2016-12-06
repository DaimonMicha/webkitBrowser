#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QObject>
#include <QWebPage>
#include <QWebFrame>
#include <QJsonDocument>

#include "chplayertable.h"



class dataParser : public QObject
{
    Q_OBJECT
public:
    explicit dataParser(chPlayerTable*,QObject *parent = 0);

    void setNetworkAccessManager(QNetworkAccessManager* manager) {
        m_parserPage->setNetworkAccessManager(manager);
    }

    QJsonDocument lastResult() const { return(m_lastResult); }

signals:

public slots:
    void clearLastResult() {
        m_lastResult.fromJson("");
    }

    void parseHtml(const QByteArray);
    void gangsterStatus(const QString, const QString);

    void opponentsListJson(const QString);
    void enemysListJson(const QString);
    void fightData(const QString);
    void getResults(const QString);
    void diaryData(const QString);
    void getBattleEventData(const QString);
    void patenvillaData(const QString);

private slots:
    void addJavaScriptObject() {
        m_parserPage->mainFrame()->addToJavaScriptWindowObject("parser", this);
    }

private:
    chPlayerTable*          m_gangstersTable;
    QMap<QString, QString>  m_cityMap;

    QWebPage*               m_parserPage;
    QJsonDocument           m_lastResult;
};

#endif // DATAPARSER_H
