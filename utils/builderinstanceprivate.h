#ifndef BUILDERINSTANCEPRIVATE_H
#define BUILDERINSTANCEPRIVATE_H

#include <QMap>
#include "manifestcontainer.h"
#include "flatpaklauncher.h"
#include "builderinstance.h"

class BuilderInstancePrivate : QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(BuilderInstance)    
protected:
    BuilderInstance* q_ptr;

public:
    enum private_event_type
    {
        none = 0,
        cui_finished = 1,
        cui_error = 2
    };
    ManifestContainer manifest;

    FlatpakLauncher fp_cui;
    FlatpakLauncher::launcher_error_code cuiErrorCode;
    int buildStage;

    QString workingDir;
    QString buildDir;
    QString repoDir;
    QString manifestPath;
    QString bundlePath;
    QString logPath;

    QString ownName;
    BuilderInstance::AppPermissions permission;

    QList<int> moduleIDs;
    QMap<int,int> sourceIDs;
    QList<ManifestContainer::ModuleProperty> moduleList;
    QList<ManifestContainer::ModuleSource> sourceList;

    BuilderInstancePrivate(BuilderInstance* parent);

    int getModuleIndexByID(int moduleID);
    int getSourceIndexByID(int sourceID);

    bool buildManifest();
    void buildRepo();
    void buildBundle();
    bool logCuiOutput(bool append);

signals:
    void privateEvent(int eventType);

public slots:
    void onCuiError(FlatpakLauncher::launcher_error_code errCode);
    void onCuiStatusChanged(CommandLauncher::launcher_status status);
};

#endif // BUILDERINSTANCEPRIVATE_H
