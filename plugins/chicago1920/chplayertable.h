#ifndef CHPLAYERTABLE_H
#define CHPLAYERTABLE_H

#include <QObject>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>




class chAccountGangster
{
public:
    QString     id;
    QString     coded_id;
    QString     name;
    QString     level;
    QString     clan;
    int         whisky;
    int         dollar;
    int         gp;
    int         max_life;
};




class chPlayerTable : public QObject
{
    Q_OBJECT
public:
    explicit chPlayerTable(QObject *parent = 0);

    void setPlayerData(QString id, QString field, QString data);
    QString getPlayerData(QString id, QString field);
    QString randomPlayer(QString race = "0");
    void midnightReset();
    void setAccountGangster(const QString id) {
        m_accountGangster.id = id;
    }

    QString accountGangster() const {
        return(m_accountGangster.id);
    }

    int countGangsters() const {
        return(m_gangstersModel->rowCount());
    }

    void clear() {
        for(int a = m_gangstersModel->rowCount(); a > 0; --a) {
            m_gangstersModel->removeRow(a-1);
        }
    }

private:
    int randInt(int low, int high) {
        return qrand() % ((high + 1) - low) + low;
    }

    int columnByName(const QString& field) {
        for(int c=0; c < m_gangstersModel->columnCount(); ++c) {
            if(field == m_gangstersModel->headerData(c, Qt::Horizontal).toString()) return(c);
        }
        return(-1);
    }

signals:

public slots:

private:
    QStandardItemModel*         m_gangstersModel;
    QSortFilterProxyModel*      m_filterModel;

    chAccountGangster           m_accountGangster;
};

#endif // CHPLAYERTABLE_H
