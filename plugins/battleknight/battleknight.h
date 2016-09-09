#ifndef BATTLEKNIGHT_H
#define BATTLEKNIGHT_H

#include "extension.h"

class BattleKnight : public QObject, public ExtensionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "de.daimon-net.gamebrowser.ExtensionInterface" FILE "battleknight.json")
    Q_INTERFACES(ExtensionInterface)

public:
    ~BattleKnight();

    bool isMyUrl(const QUrl &) const;

    void loadSettings(QSettings &);
    void loadStarted(WebPage*,const QUrl &);
    void loadFinished(WebPage*);
    void loadFinished(QNetworkReply*);
};

#endif // BATTLEKNIGHT_H
