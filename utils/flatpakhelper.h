#ifndef FLATPAKHELPER_H
#define FLATPAKHELPER_H

#include <QString>
#include <QList>


class FlatpakHelperPrivate;

class FlatpakHelper
{
public:
    struct RuntimeInfo
    {
        QString name;
        QString version;
    };

    FlatpakHelper();
    static QList<RuntimeInfo> getRuntimeList(QString flatpakDir = "");
    static QList<RuntimeInfo> getSDKList(QString flatpakDir = "");
};

#endif // FLATPAKHELPER_H
