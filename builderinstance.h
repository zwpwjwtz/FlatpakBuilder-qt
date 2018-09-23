#ifndef BUILDERINSTANCE_H
#define BUILDERINSTANCE_H

#include <QObject>
#include <QVector>

class BuilderInstancePrivate;

class BuilderInstance : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(BuilderInstance)
protected:
    BuilderInstancePrivate* d_ptr;

public:
    enum AppPermission
    {
        NoPermission = 0,
        AllowIPC = 1,
        X11Access = 2,
        AudioAccess = 4,
        NetworkAccess = 8,
        SystemBusAccess = 16,
        Device_DRI = 32,
        FS_ReadWrite = 64,
        FS_HomeDir = 128,
        FS_RootDir = 256
    };
    Q_DECLARE_FLAGS(AppPermissions, AppPermission)
    enum SourceType
    {
        UnknownSource = 0,
        ArchiveSource = 1,
        GitSource = 2,
        BzrSource = 3,
        FileSource = 4,
        ScriptSource = 5,
        ShellSource = 6,
        PatchSource = 7
    };
    enum BuilderErrorCode
    {
        ok = 0,
        manifest_not_generated = 1,
        working_dir_not_exists = 2,
        repo_not_exists = 3,
        build_dir_not_exists = 4,
        exe_not_exists = 129,
        no_permission = 130,
        unknownError = 255,
    };
    enum BuilderStage
    {
        Wait = 0,
        BuildManifest = 1,
        BuildRepo = 2,
        BuildBundle = 3
    };

    explicit BuilderInstance();
    void build();

    int addModule();
    void removeModule(int moduleID);
    QVector<int> moduleList();
    QString moduleName(int moduleID);
    void setModuleName(int moduleID, QString moduleName);

    int addSource(int moduleID);
    void removeSource(int sourceID);
    QVector<int> sourceList(int moduleID);
    SourceType sourceType(int sourceID);
    void setSourceType(int sourceID, SourceType srcType);
    static QString sourceTypeToString(SourceType srcType);
    QString sourceURL(int sourceID);
    void setSourceURL(int sourceID, QString url);
    QString sourceVer(int sourceID);
    void setSourceVer(int sourceID, QString version);
    QString sourceCmd(int sourceID);
    void setSourceCmd(int sourceID, QString command);

    Q_PROPERTY(QString flatpakPath READ flatpakPath WRITE setFlatpakPath)
    Q_PROPERTY(QString flatpakBuilderPath READ flatpakBuilderPath
                                          WRITE setFlatpakBuilderPath)
    Q_PROPERTY(QString workingDir READ workingDir WRITE setWorkingDir)
    Q_PROPERTY(QString buildDir READ buildDir WRITE setBuildDir)
    Q_PROPERTY(QString repoDir READ repoDir WRITE setRepoDir)
    Q_PROPERTY(QString bundleFile READ bundleFile WRITE setBundleFile)
    Q_PROPERTY(QString logFile READ logFile WRITE setLogFile)

    Q_PROPERTY(QString appID READ appID WRITE setAppID)
    Q_PROPERTY(QString branch READ branch WRITE setBranch)
    Q_PROPERTY(QString runtime READ runtime WRITE setRuntime)
    Q_PROPERTY(QString runtimeVer READ runtimeVer WRITE setRuntimeVer)
    Q_PROPERTY(QString sdkName READ sdkName WRITE setSdkName)
    Q_PROPERTY(QString runCmd READ runCmd WRITE setRunCmd)
    Q_PROPERTY(QString ownName READ ownName WRITE setOwnName)
    Q_PROPERTY(AppPermissions permission READ permission WRITE setPermission)

    QString flatpakPath();
    void setFlatpakPath(QString path);
    QString flatpakBuilderPath();
    void setFlatpakBuilderPath(QString path);
    QString workingDir();
    void setWorkingDir(QString dir);
    QString buildDir();
    void setBuildDir(QString dir);
    QString repoDir();
    void setRepoDir(QString dir);
    QString bundleFile();
    void setBundleFile(QString filePath);
    QString logFile();
    void setLogFile(QString filePath);

    QString appID();
    void setAppID(const QString appID);
    QString branch();
    void setBranch(const QString branchName);
    QString runtime();
    void setRuntime(const QString runtime);
    QString runtimeVer();
    void setRuntimeVer(const QString version);
    QString sdkName();
    void setSdkName(const QString sdk);
    QString runCmd();
    void setRunCmd(const QString command);
    QString ownName();
    void setOwnName(const QString name);
    AppPermissions permission();
    void setPermission(const AppPermissions permission);

signals:
    void builder_error(BuilderErrorCode errCode);
    void builder_staged(BuilderStage stage);
    void builder_finished();

private slots:
    void onPrivateEvent(int eventType);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(BuilderInstance::AppPermissions)

#endif // BUILDERINSTANCE_H
