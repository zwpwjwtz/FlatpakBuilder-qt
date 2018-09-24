#ifndef MANIFESTCONTAINERPRIVATE_H
#define MANIFESTCONTAINERPRIVATE_H

/*
 * Data structure defined according to
 * Flatpak version 0.10.2
 * Please refer to Flatpak-builder manual for
 * more details and help support more parameters
 */

#include "manifestcontainer.h"


class ManifestContainerPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ManifestContainer)

public:

    QString appID;
    QString branchName;
    QString runtimeName;
    QString runtimeVer;
    QString sdkName;
    QString command;
    ManifestContainer::AppBuildOptions buildOptions;
    bool isLocaleSeparated;
    QList<QString> tags;
    QList<QString> finishArgs;
    QList<ManifestContainer::ModuleProperty> modules;

protected:
    ManifestContainer* q_ptr;
};

#endif // MANIFESTCONTAINERPRIVATE_H
