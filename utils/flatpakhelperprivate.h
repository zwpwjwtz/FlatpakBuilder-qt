#ifndef FLATPAKHELPERPRIVATE_H
#define FLATPAKHELPERPRIVATE_H

#include "flatpakhelper.h"


class FlatpakHelperPrivate
{
public:
    static QString findFirstDirEntry(QString path);
    static QList<QString> getDirectoryEntry(QString path);
    static QList<FlatpakHelper::RuntimeInfo> getRuntimeList(QString flatpakDir);
};
#endif // FLATPAKHELPERPRIVATE_H
