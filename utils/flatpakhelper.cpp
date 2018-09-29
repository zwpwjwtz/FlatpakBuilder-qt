#include "flatpakhelper.h"
#include "flatpakhelperprivate.h"
#include <QFileInfo>
#include <QDir>

#define FPBDQT_LIBRARY_FLATPAK_RUNTIME "/var/lib/flatpak/runtime"


FlatpakHelper::FlatpakHelper()
{
}

QList<FlatpakHelper::RuntimeInfo>
FlatpakHelper::getRuntimeList(QString flatpakDir)
{
    QList<RuntimeInfo> runtimeList =
                            FlatpakHelperPrivate::getRuntimeList(flatpakDir);
    for (int i=0; i<runtimeList.count(); i++)
    {
        if (runtimeList[i].name.contains(".Sdk", Qt::CaseInsensitive))
            runtimeList.removeAt(i--);
    }
    return runtimeList;
}

QList<FlatpakHelper::RuntimeInfo>
FlatpakHelper::getSDKList(QString flatpakDir)
{
    QList<RuntimeInfo> sdkList =
                            FlatpakHelperPrivate::getRuntimeList(flatpakDir);
    for (int i=0; i<sdkList.count(); i++)
    {
        if (!sdkList[i].name.contains(".Sdk", Qt::CaseInsensitive))
            sdkList.removeAt(i--);
    }
    return sdkList;
}

QString FlatpakHelperPrivate::findFirstDirEntry(QString path)
{
    QString found;

    QDir dir(path);
    QFileInfo fileInfo;
    for (int i=0; i<dir.entryList().count(); i++)
    {
        if (dir.entryList()[i] == "." || dir.entryList()[i] == "..")
            continue;
        fileInfo.setFile(dir, dir.entryList()[i]);
        if (fileInfo.isReadable() && fileInfo.isDir())
        {
            found = dir.entryList()[i];
            break;
        }
    }
    return found;
}

QList<FlatpakHelper::RuntimeInfo>
FlatpakHelperPrivate::getRuntimeList(QString flatpakDir)
{
    if (flatpakDir.isEmpty())
        flatpakDir = FPBDQT_LIBRARY_FLATPAK_RUNTIME;

    // Read runtime directory and get the first entry
    QList<FlatpakHelper::RuntimeInfo> runtimeList;
    QList<QString> dirs = FlatpakHelperPrivate::getDirectoryEntry(flatpakDir);
    for (int i=0; i<dirs.count(); i++)
    {
        QString dirName;
        FlatpakHelper::RuntimeInfo runtime;

        // Read the first entry of each entry we have found
        // It should be a directory having name like "x86_64"
        runtime.name = dirs[i];
        dirName = QString(flatpakDir).append('/').append(runtime.name);
        dirName = FlatpakHelperPrivate::findFirstDirEntry(dirName);
        if (dirName.isEmpty())
            continue;

        // Read the first entry in the third level directory
        // It should be a directory having a version-like name
        dirName = QString(flatpakDir)
                         .append('/').append(runtime.name)
                         .append('/').append(dirName);
        dirName = FlatpakHelperPrivate::findFirstDirEntry(dirName);
        if (dirName.isEmpty())
            continue;

        runtime.version = dirName;
        runtimeList.push_back(runtime);
    }

    return runtimeList;
}

QList<QString> FlatpakHelperPrivate::getDirectoryEntry(QString path)
{
    QDir dir(path);
    QFileInfo fileInfo;
    QList<QString> dirList;
    for (int i=0; i<dir.entryList().count(); i++)
    {
        if (dir.entryList()[i] == "." || dir.entryList()[i] == "..")
            continue;
        fileInfo.setFile(dir, dir.entryList()[i]);
        if (fileInfo.isReadable() && fileInfo.isDir())
            dirList.push_back(dir.entryList()[i]);
    }
    return dirList;
}
