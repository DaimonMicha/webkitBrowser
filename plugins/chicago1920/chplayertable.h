#ifndef CHPLAYERTABLE_H
#define CHPLAYERTABLE_H

#include <QObject>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class chPlayerTable : public QObject
{
    Q_OBJECT
public:
    explicit chPlayerTable(QObject *parent = 0);

    int countGangsters() const { return(m_gangstersModel->rowCount()); }
    void setPlayerData(QString id, QString field, QString data);
    QString getPlayerData(QString id, QString field);
    QString randomPlayer(QString race = "0");
    void midnightReset();

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

};

#endif // CHPLAYERTABLE_H
