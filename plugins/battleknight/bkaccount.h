#ifndef BKACCOUNT_H
#define BKACCOUNT_H

#include <QObject>
#include <QWebPage>


class botConfig
{
public:
    bool bot;
};


class bkAccount : public QObject
{
    Q_OBJECT
public:
    bkAccount(const QString cookie, QObject *parent = 0);

    Q_INVOKABLE bool isActive(const QString option = "account") const {
        if(option == "account") return(m_config.bot);
        return(false);
    }
    Q_INVOKABLE QString cookieValue() const { return(m_cookieValue); }

    void loadFinished(QWebPage*);

signals:

public slots:
    void toggle(const QString option = "account", const bool on = false);

private:
    botConfig               m_config;
    QString                 m_cookieValue;
};

#endif // BKACCOUNT_H
