#include "chplayertable.h"

#include <QDebug>



chPlayerTable::chPlayerTable(QObject *parent) :
    QObject(parent),
    m_gangstersModel(new QStandardItemModel),
    m_filterModel(new QSortFilterProxyModel)
{
    QStringList head;
/*
    "id": "272925",           // VipList
    "coded_id": "BHDLFJ",     // VipList
    "name": "Derseekin2000",  // VipList
    "level": "17",            // VipList
    "race_id": "7",           // KB
    "race": "Ashburn",        // VipList - aus interner Tabelle holen.
    "fightlimit": false,      // VipList - wird berechnet?
    "rang": 60,               // VipList
    "win": "37"               // VipList
    "lost": "0",              // VipList
    "gold": "0",              // VipList (Beute gesamt)
    "magicDamage": 101,       // KB (Erniedrigung)
    "maxLp": 474,             // KB
    "platting": 73,           // KB (Rüstung)
    "minDamage": 7,           // KB
    "maxDaqmage": 13          // KB
*/
    head    << "id"
            << "coded_id"
            << "name"
            << "level"
            << "race_id"
            << "clan_id"
            << "fightlimit"
            << "fightsDone"
            << "fightsMax"
            << "rang"
            << "win"
            << "lost"
            << "gold"
            << "maxLP"
            << "platting"
            << "minDamage"
            << "maxDamage"
            << "magicDamage"
            << "type";
    m_gangstersModel->setHorizontalHeaderLabels(head);
    m_filterModel->setSourceModel(m_gangstersModel);
    m_filterModel->setDynamicSortFilter(true);
}

void chPlayerTable::midnightReset()
{
    for(int a = 0; a < m_gangstersModel->rowCount(); ++a) {
        m_gangstersModel->item(a,columnByName("fightlimit"))->setText("false");
        m_gangstersModel->item(a,columnByName("fightsDone"))->setText("0");
        if(m_gangstersModel->item(a, columnByName("name"))->text() == "Polizei") {
            m_gangstersModel->removeRow(a);
        }
    }
}

void chPlayerTable::setPlayerData(QString id, QString field, QString data)
{
    int col = columnByName(field);
    if(col < 0) {
        if(id == m_accountGangster.id) {
            if(field == "whisky") m_accountGangster.whisky = data.toInt();
            else if(field == "dollar") m_accountGangster.dollar = data.toInt();
            else if(field == "gp") m_accountGangster.gp = data.toInt();
        }
        return;
    }
    QStandardItem *playerItem;
    QList<QStandardItem *> rows = m_gangstersModel->findItems(id,Qt::MatchExactly,columnByName("id"));
    if(rows.count() == 0) {
        QList<QStandardItem *> items;
        playerItem = new QStandardItem(id);
        items.append(playerItem);
        for(int c = 1; c < m_gangstersModel->columnCount(); ++c) {
            items.append(new QStandardItem());
        }
        m_gangstersModel->appendRow(items);
    } else {
        playerItem = rows.at(0);
    }
    int row = playerItem->row();

    // check row
    playerItem = m_gangstersModel->item(row, col);
    if(playerItem) playerItem->setText(data);
}

QString chPlayerTable::getPlayerData(QString id, QString field)
{
    QString ret;
    int col = columnByName(field);
    if(col < 0) {
        if(id == m_accountGangster.id) {
            if(field == "whisky") ret = QString("%1").arg(m_accountGangster.whisky);
            else if(field == "dollar") ret = QString("%1").arg(m_accountGangster.dollar);
            else if(field == "gp") ret = QString("%1").arg(m_accountGangster.gp);
        }
        return(ret);
    }
    QStandardItem *playerItem;
    QList<QStandardItem *> rows = m_gangstersModel->findItems(id);
    if(rows.count() == 0) {
        return(ret);
    } else {
        playerItem = rows.at(0);
    }
    int row = playerItem->row();
    return(m_gangstersModel->item(row, col)->text());
}

QString chPlayerTable::randomPlayer(QString race)
{
    //ToDo: filtern auf fightlimit != true
    QString ret;
    QSortFilterProxyModel* typeModel = new QSortFilterProxyModel();
    typeModel->setSourceModel(m_gangstersModel);
    typeModel->setFilterKeyColumn(columnByName("type"));
    typeModel->setFilterFixedString("reputation");
    typeModel->setDynamicSortFilter(true);

    QSortFilterProxyModel* fightlimitModel = new QSortFilterProxyModel();
    fightlimitModel->setSourceModel(typeModel);
    fightlimitModel->setFilterKeyColumn(columnByName("fightlimit"));
    fightlimitModel->setFilterFixedString("false");
    fightlimitModel->setDynamicSortFilter(true);

    m_filterModel->setSourceModel(fightlimitModel);

    if(m_filterModel->rowCount() == 0) {
        m_filterModel->setFilterFixedString("");
        m_filterModel->setSourceModel(m_gangstersModel);
        return(ret);
    }

    if(race != "0") {
        m_filterModel->setFilterKeyColumn(columnByName("race_id"));
        m_filterModel->setFilterFixedString(race);
    }

    if(m_filterModel->rowCount() == 0) {
        m_filterModel->setFilterFixedString("");
        m_filterModel->setSourceModel(m_gangstersModel);
        return(ret);
    }

    int row = randInt(1, m_filterModel->rowCount()) - 1;
    QModelIndex idx = m_filterModel->index(row,columnByName("id"));
    ret = idx.data().toString();

    m_filterModel->setFilterFixedString("");
    m_filterModel->setSourceModel(m_gangstersModel);
    return(ret);
}
