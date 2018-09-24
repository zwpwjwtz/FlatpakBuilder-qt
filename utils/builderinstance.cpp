#include <QFile>

#include "builderinstance.h"
#include "builderinstanceprivate.h"
#include "manifest_format.h"

#define FPBDQT_BUILDER_MANIFEST_DEFAULT "manifest.json"

BuilderInstance::BuilderInstance()
{
    this->d_ptr = new BuilderInstancePrivate(this);
    connect(this->d_ptr,
            SIGNAL(privateEvent(int)),
            this,
            SLOT(onPrivateEvent(int)));
}

void BuilderInstance::build()
{
    Q_D(BuilderInstance);
    switch (d->buildStage)
    {
        case 0:
            d->buildStage++;
            emit builder_staged(BuildManifest);
            d->buildManifest();
        case 1:
            d->buildStage++;
            emit builder_staged(BuildRepo);
            d->buildRepo();
            break;
        case 2:
            d->logCuiOutput(false);
            d->buildStage++;
            emit builder_staged(BuildBundle);
            d->buildBundle();
            break;
        case 3:
            d->logCuiOutput(true);
            d->buildStage = 0;
            emit builder_finished();
        default:;
    }
}

int BuilderInstance::addModule()
{
    Q_D(BuilderInstance);

    ManifestContainer::ModuleProperty module;
    d->moduleList.push_back(module);

    if (d->moduleIDs.count() < 1)
        d->moduleIDs.push_back(1);
    else
        d->moduleIDs.push_back(d->moduleIDs.last() + 1);
    return d->moduleIDs.last();
}

void BuilderInstance::removeModule(int moduleID)
{
    Q_D(BuilderInstance);

    int index = d->getModuleIndexByID(moduleID);
    d->moduleList.removeAt(index);
    d->moduleIDs.removeAt(index);
}

QVector<int> BuilderInstance::moduleList()
{
    Q_D(BuilderInstance);
    return d->moduleIDs.toVector();
}

int BuilderInstance::addSource(int moduleID)
{
    Q_D(BuilderInstance);
    ManifestContainer::ModuleSource source;
    d->sourceList.push_back(source);

    // Here we update sourceID list only.
    // We do not really add this source to the module,
    // until we need to generate a json file with ManifestContainer
    int lastID;
    if (d->sourceIDs.count() < 1)
        lastID = 0;
    else

        lastID = (--(d->sourceIDs.constEnd())).key();
    d->sourceIDs[lastID + 1] = moduleID;
    return lastID + 1;
}

void BuilderInstance::removeSource(int sourceID)
{
    Q_D(BuilderInstance);

    // Same as the addSource()
    d->sourceList.removeAt(d->getSourceIndexByID(sourceID));
    d->sourceIDs.remove(sourceID);
}

QVector<int> BuilderInstance::sourceList(int moduleID)
{
    Q_D(BuilderInstance);
    QList<int> tempList;
    QMap<int,int>::const_iterator i;
    for (i=d->sourceIDs.constBegin(); i!=d->sourceIDs.constEnd(); i++)
    {
        if (i.value() == moduleID)
            tempList.push_back(i.key());
    }
    return tempList.toVector();
}

QString BuilderInstance::flatpakPath()
{
    Q_D(BuilderInstance);
    return d->fp_cui.flatpakPath();
}

void BuilderInstance::setFlatpakPath(QString path)
{
    Q_D(BuilderInstance);
    d->fp_cui.setFlatpakPath(path);
}

QString BuilderInstance::flatpakBuilderPath()
{
    Q_D(BuilderInstance);
    return d->fp_cui.builderPath();
}

void BuilderInstance::setFlatpakBuilderPath(QString path)
{
    Q_D(BuilderInstance);
    d->fp_cui.setBuilderPath(path);
}

QString BuilderInstance::workingDir()
{
    Q_D(BuilderInstance);
    return d->workingDir;
}

void BuilderInstance::setWorkingDir(QString dir)
{
    Q_D(BuilderInstance);
    d->workingDir = dir;
}

QString BuilderInstance::buildDir()
{
    Q_D(BuilderInstance);
    return d->buildDir;
}

void BuilderInstance::setBuildDir(QString dir)
{
    Q_D(BuilderInstance);
    d->buildDir = dir;
}

QString BuilderInstance::repoDir()
{
    Q_D(BuilderInstance);
    return d->repoDir;
}

void BuilderInstance::setRepoDir(QString dir)
{
    Q_D(BuilderInstance);
    d->repoDir = dir;
}

QString BuilderInstance::bundleFile()
{
    Q_D(BuilderInstance);
    return d->bundlePath;
}

void BuilderInstance::setBundleFile(QString filePath)
{
    Q_D(BuilderInstance);
    d->bundlePath = filePath;
}

QString BuilderInstance::logFile()
{
    Q_D(BuilderInstance);
    return d->logPath;
}

void BuilderInstance::setLogFile(QString filePath)
{
    Q_D(BuilderInstance);
    d->logPath = filePath;
}

QString BuilderInstance::appID()
{
    Q_D(BuilderInstance);
    return d->manifest.appID();
}

void BuilderInstance::setAppID(const QString appID)
{
    Q_D(BuilderInstance);
    d->manifest.setAppID(appID);
}

QString BuilderInstance::branch()
{
    Q_D(BuilderInstance);
    return d->manifest.branch();
}

void BuilderInstance::setBranch(const QString branchName)
{
    Q_D(BuilderInstance);
    d->manifest.setBranch(branchName);
}

QString BuilderInstance::runtime()
{
    Q_D(BuilderInstance);
    return d->manifest.runtime();
}

void BuilderInstance::setRuntime(const QString runtime)
{
    Q_D(BuilderInstance);
    d->manifest.setRuntime(runtime);
}

QString BuilderInstance::runtimeVer()
{
    Q_D(BuilderInstance);
    return d->manifest.runtimeVer();
}

void BuilderInstance::setRuntimeVer(const QString version)
{
    Q_D(BuilderInstance);
    d->manifest.setRuntimeVer(version);
}

QString BuilderInstance::sdkName()
{
    Q_D(BuilderInstance);
    return d->manifest.sdkName();
}

void BuilderInstance::setSdkName(const QString sdk)
{
    Q_D(BuilderInstance);
    d->manifest.setSdkName(sdk);
}

QString BuilderInstance::runCmd()
{
    Q_D(BuilderInstance);
    return d->manifest.command();
}

void BuilderInstance::setRunCmd(const QString command)
{
    Q_D(BuilderInstance);
    d->manifest.setCommand(command);
}

QString BuilderInstance::ownName()
{
    Q_D(BuilderInstance);
    return d->ownName;
}

void BuilderInstance::setOwnName(const QString name)
{
    Q_D(BuilderInstance);
    d->ownName = name;
}

BuilderInstance::AppPermissions BuilderInstance::permission()
{
    Q_D(BuilderInstance);
    return d->permission;
}

void BuilderInstance::setPermission(const AppPermissions permission)
{
    Q_D(BuilderInstance);
    d->permission = permission;
}

QString BuilderInstance::moduleName(int moduleID)
{
    Q_D(BuilderInstance);
    return d->moduleList[d->moduleIDs.indexOf(moduleID)].name;
}

void BuilderInstance::setModuleName(int moduleID, QString moduleName)
{
    Q_D(BuilderInstance);
    d->moduleList[d->moduleIDs.indexOf(moduleID)].name = moduleName;
}

BuilderInstance::SourceType BuilderInstance::sourceType(int sourceID)
{
    Q_D(BuilderInstance);

    SourceType typeValue;
    QString typeString = d->sourceList[d->getSourceIndexByID(sourceID)].type;
    if (typeString == FLATPAK_MANIFEST_SOURCE_TYPE_ARCHIVE)
        typeValue = ArchiveSource;
    else if (typeString == FLATPAK_MANIFEST_SOURCE_TYPE_GIT)
        typeValue = GitSource;
    else if (typeString == FLATPAK_MANIFEST_SOURCE_TYPE_BZR)
        typeValue = BzrSource;
    else if (typeString == FLATPAK_MANIFEST_SOURCE_TYPE_FILE)
        typeValue = FileSource;
    else if (typeString == FLATPAK_MANIFEST_SOURCE_TYPE_SCRIPT)
        typeValue = ScriptSource;
    else if (typeString == FLATPAK_MANIFEST_SOURCE_TYPE_SHELL)
        typeValue = ShellSource;
    else if (typeString == FLATPAK_MANIFEST_SOURCE_TYPE_PATCH)
        typeValue = PatchSource;
    else
        typeValue = UnknownSource;

    return typeValue;
}

void BuilderInstance::setSourceType(int sourceID, SourceType srcType)
{
    Q_D(BuilderInstance);

    QString typeString;
    switch (srcType)
    {
        case ArchiveSource:
            typeString = FLATPAK_MANIFEST_SOURCE_TYPE_ARCHIVE;
            break;
        case GitSource:
            typeString = FLATPAK_MANIFEST_SOURCE_TYPE_GIT;
            break;
        case BzrSource:
            typeString = FLATPAK_MANIFEST_SOURCE_TYPE_BZR;
            break;
        case FileSource:
            typeString = FLATPAK_MANIFEST_SOURCE_TYPE_FILE;
            break;
        case ScriptSource:
            typeString = FLATPAK_MANIFEST_SOURCE_TYPE_SCRIPT;
            break;
        case ShellSource:
            typeString = FLATPAK_MANIFEST_SOURCE_TYPE_SHELL;
            break;
        case PatchSource:
            typeString = FLATPAK_MANIFEST_SOURCE_TYPE_PATCH;
            break;
        default:;
    }

    if (!typeString.isEmpty())
        d->sourceList[d->getSourceIndexByID(sourceID)].type = typeString;
}

QString BuilderInstance::sourceTypeToString(SourceType srcType)
{
    QString typeString;
    switch (srcType)
    {
        case ArchiveSource:
            typeString = "Archive File";
            break;
        case GitSource:
            typeString = "Git Repo";
            break;
        case BzrSource:
            typeString = "Bzr Repo";
            break;
        case FileSource:
            typeString = "Single File";
            break;
        case ScriptSource:
            typeString = "Script File";
            break;
        case ShellSource:
            typeString = "Shell Command";
            break;
        case PatchSource:
            typeString = "Patch File";
            break;
        default:
            typeString = "Unknown";
    }
    return typeString;
}

QString BuilderInstance::sourceURL(int sourceID)
{
    Q_D(BuilderInstance);
    switch (sourceType(sourceID))
    {
        case ScriptSource:
        case ShellSource:
            return d->sourceList[d->getSourceIndexByID(sourceID)].destFilename;
        case ArchiveSource:
        case GitSource:
        case BzrSource:
        case FileSource:
        case PatchSource:
        default:
            return d->sourceList[d->getSourceIndexByID(sourceID)].url;
    }
}

void BuilderInstance::setSourceURL(int sourceID, QString url)
{
    Q_D(BuilderInstance);
    switch (sourceType(sourceID))
    {
        case ScriptSource:
        case ShellSource:
            d->sourceList[d->getSourceIndexByID(sourceID)].destFilename = url;
            break;
        case ArchiveSource:
        case GitSource:
        case BzrSource:
        case FileSource:
        case PatchSource:
        default:
            d->sourceList[d->getSourceIndexByID(sourceID)].url = url;
    }

}

QString BuilderInstance::sourceVer(int sourceID)
{
    Q_D(BuilderInstance);
    return d->sourceList[d->getSourceIndexByID(sourceID)].branch;
}

void BuilderInstance::setSourceVer(int sourceID, QString version)
{
    Q_D(BuilderInstance);
    d->sourceList[d->getSourceIndexByID(sourceID)].branch = version;
}

QString BuilderInstance::sourceCmd(int sourceID)
{
    Q_D(BuilderInstance);
    return d->sourceList[d->getSourceIndexByID(sourceID)].commands.join('\n');
}

void BuilderInstance::setSourceCmd(int sourceID, QString command)
{
    Q_D(BuilderInstance);
    d->sourceList[d->getSourceIndexByID(sourceID)].commands =
            command.split('\n', QString::SkipEmptyParts).toVector().toList();
}

void BuilderInstance::onPrivateEvent(int eventType)
{
    Q_D(BuilderInstance);
    switch (eventType)
    {
        case BuilderInstancePrivate::cui_finished:
            build();
            break;
        case BuilderInstancePrivate::cui_error:
        {
            switch (d->cuiErrorCode)
            {
                case FlatpakLauncher::ok:
                    emit builder_error(ok);
                    break;
                case FlatpakLauncher::manifest_not_exists:
                    emit builder_error(manifest_not_generated);
                    break;
                case FlatpakLauncher::repo_not_exists:
                    emit builder_error(repo_not_exists);
                    break;
                case FlatpakLauncher::build_dir_not_exists:
                    emit builder_error(build_dir_not_exists);
                    break;
                case FlatpakLauncher::exe_not_exists:
                    emit builder_error(exe_not_exists);
                    break;
                case FlatpakLauncher::no_permission:
                    emit builder_error(no_permission);
                    break;
                case FlatpakLauncher::unknownError:
                    emit builder_error(unknownError);
                    break;
                default:;
            }
        }
        default:;
    }
}

BuilderInstancePrivate::BuilderInstancePrivate(BuilderInstance* parent)
{
    this->q_ptr = parent;
    buildStage = 0;

    connect(&fp_cui,
            SIGNAL(launcher_status_changed(FlatpakLauncher::launcher_status)),
            this,
            SLOT(onCuiStatusChanged(FlatpakLauncher::launcher_status)));
}

int BuilderInstancePrivate::getModuleIndexByID(int moduleID)
{
    return moduleIDs.indexOf(moduleID);
}

int BuilderInstancePrivate::getSourceIndexByID(int sourceID)
{
    int count = 0;
    QMap<int,int>::const_iterator i;
    for (i=sourceIDs.constBegin(); i!= sourceIDs.constEnd(); i++)
    {
        if (i.key() == sourceID)
            return count;
        else
            count++;
    }
    return -1;
}

bool BuilderInstancePrivate::buildManifest()
{
    if (moduleList.count() < 1 || sourceList.count() < 1)
        return false;

    QMap<int,int>::const_iterator j;
    ManifestContainer::ModuleProperty module;
    manifest.clearModules();
    for (int i=0; i<moduleList.count(); i++)
    {
        // Add sources to module
        int count = 0;
        module = moduleList[i];
        for (j=sourceIDs.constBegin(); j!=sourceIDs.constEnd(); j++)
        {
            if (j.value() == moduleIDs[i])
                module.sources.push_back(sourceList[count]);
            count++;
        }

        manifest.addModules(module);
    }

    QList<QString> finishArgs;
    if (permission & BuilderInstance::AllowIPC)
        finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_SHARE_IPC);
    if (permission & BuilderInstance::X11Access)
        finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_SOCKET_X11);
    if (permission & BuilderInstance::AudioAccess)
        finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_SOCKET_AUDIO);
    if (permission & BuilderInstance::NetworkAccess)
        finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_SHARE_NET);
    if (permission & BuilderInstance::SystemBusAccess)
        finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_SOCKET_DBUS);
    if (permission & BuilderInstance::Device_DRI)
        finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_DEVICE_DRI);
    if (permission & BuilderInstance::FS_RootDir)
    {
        if (permission & BuilderInstance::FS_ReadWrite)
            finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_FS_HOST);
        else
            finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_FS_HOST
                                 FLATPAK_MANIFEST_FINARGS_FS_READONLY);
    }
    else if (permission & BuilderInstance::FS_HomeDir)
    {
        if (permission & BuilderInstance::FS_ReadWrite)
            finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_FS_HOME);
        else
            finishArgs.push_back(FLATPAK_MANIFEST_FINARGS_FS_HOME
                                 FLATPAK_MANIFEST_FINARGS_FS_READONLY);
    }
    if (!ownName.isEmpty())
        finishArgs.push_back(QString(FLATPAK_MANIFEST_FINARGS_OWN_NAME)
                             .append(ownName));
    manifest.setFinishArgs(finishArgs);

    // Write the manifest to file
    manifestPath = QString(workingDir)
                          .append('/')
                          .append(FPBDQT_BUILDER_MANIFEST_DEFAULT);
    QFile jsonFile(manifestPath);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(manifest.toJson());
    jsonFile.close();

    return true;
}

void BuilderInstancePrivate::buildRepo()
{
    // Launch flatpak-builder
    fp_cui.setRepoDirectory(repoDir);
    fp_cui.setWorkingDirectory(workingDir);
    fp_cui.build(manifestPath, buildDir);
}

void BuilderInstancePrivate::buildBundle()
{
    // Launch flatpak to build a bundle file
    fp_cui.setAppName(manifest.appID());
    fp_cui.buildBundle(bundlePath);
}

bool BuilderInstancePrivate::logCuiOutput(bool append)
{
    QFile log(logPath);
    if (append)
        log.open(QFile::Append);
    else
        log.open(QFile::WriteOnly);
    if (!log.isOpen())
        return false;

    log.write(fp_cui.output());
    log.close();
    return true;
}

void BuilderInstancePrivate::onCuiStatusChanged(
                                    FlatpakLauncher::launcher_status status)
{
    if (status == FlatpakLauncher::finished)
        emit privateEvent(cui_finished);
}

void BuilderInstancePrivate::onCuiError(
                                FlatpakLauncher::launcher_error_code errCode)
{
    cuiErrorCode = errCode;
    emit privateEvent(cui_error);
}
