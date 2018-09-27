#ifndef BUILDERINSTANCEPRIVATE_H
#define BUILDERINSTANCEPRIVATE_H

#include <QMap>
#include "manifestcontainer.h"
#include "flatpaklauncher.h"
#include "gcclauncher.h"
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
        fp_cui_finished = 1,
        fp_cui_error = 2,
        gcc_cui_finished = 3,
        gcc_cui_error = 4
    };
    ManifestContainer manifest;

    FlatpakLauncher fp_cui;
    FlatpakLauncher::launcher_error_code fpCuiErrorCode;
    GCCLauncher gcc_cui;
    GCCLauncher::ErrorCode gccCuiErrorCode;
    int buildStage;
    bool makingExecutable;

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
    void buildExecutableHeader(bool preRun = false);
    void buildExecutable();
    bool logCuiOutput(bool append);

signals:
    void privateEvent(int eventType);

public slots:
    void onFlatpakCuiError(FlatpakLauncher::launcher_error_code errCode);
    void onFlatpakCuiStatusChanged(CommandLauncher::launcher_status status);
    void onGCCCuiError(GCCLauncher::ErrorCode errCode);
    void onGCCCuiStatusChanged(CommandLauncher::launcher_status status);
};

#endif // BUILDERINSTANCEPRIVATE_H
