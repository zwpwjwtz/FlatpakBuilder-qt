#include <QFile>
#include <QVariant>

#include "builderinstance.h"
#include "builderinstanceprivate.h"
#include "manifest_format.h"
#include "checksumcalculator.h"

#define FPBDQT_BUILDER_MANIFEST_DEFAULT "manifest.json"

#define FPBDQT_BUILDER_COMPILER_GCC_PATH "/usr/bin/gcc"

#define FPBDQT_BUILDER_INSTALLER_RESOURCES ":/installer/header.c\n" \
                                           ":/installer/header.h"
#define FPBDQT_BUILDER_INSTALLER_SOURCES "header.c"
#define FPBDQT_BUILDER_INSTALLER_EXEC_HEADER "header"


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

    bool finished = false;
    switch (d->buildStage)
    {
        case 0:
            d->logCuiOutput(false);
            d->timer.start();

            d->buildStage++;
            emit builder_staged(PrepareSource);
            d->manifest.clearModules();
            if (!d->prepareSource())
                break;
        case 1:
            d->buildStage++;
            emit builder_staged(BuildManifest);
            if (!d->buildManifest())
            {
                finished = true;
                break;
            }
        case 2:
            d->buildStage++;
            emit builder_staged(BuildRepo);
            d->buildRepo();
            break;
        case 3:
            d->buildStage++;
            emit builder_staged(BuildBundle);
            d->buildBundle();
            break;
        case 4:
            if (d->makingExecutable)
            {
                d->buildStage++;
                emit builder_staged(BuildExecutable);
                d->buildExecutableHeader(true);
            }
            else
                finished = true;
            break;
        case 5:
            d->buildStage++;
            d->buildExecutableHeader();
            break;
        case 6:
            d->buildStage++;
            d->buildExecutable();
            break;
        case 7:
            finished = true;
        default:;
    }
    if (finished)
    {
        d->timer.stop();
        d->buildStage = 0;
        d->logCuiOutput();
        emit builder_finished();
    }
}

bool BuilderInstance::makingExecutable()
{
    Q_D(BuilderInstance);
    return d->makingExecutable;
}

void BuilderInstance::setMakingExecutable(bool executable)
{
    Q_D(BuilderInstance);
    d->makingExecutable = executable;
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
    else if (typeString == FLATPAK_MANIFEST_SOURCE_TYPE_DIRECTORY)
        typeValue = DirectorySource;
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
        case DirectorySource:
            typeString = FLATPAK_MANIFEST_SOURCE_TYPE_DIRECTORY;
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
        case DirectorySource:
            typeString = "Directory";
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
        case BuilderInstancePrivate::compressor_finished:
            if (d->prepareSource())
                build();
            else
                d->prepareSource();
            break;
        case BuilderInstancePrivate::compressor_error:
            switch (d->compressorErrorCode)
            {
                case CompressorLauncher::ok:
                    emit builder_error(ok);
                    break;
                case CompressorLauncher::command_not_exists:
                    emit builder_error(compressor_not_working);
                    break;
                case CompressorLauncher::unknownError:
                default:
                    emit builder_error(unknownError);
                    break;
            }
            d->timer.stop();
            break;
        case BuilderInstancePrivate::fp_cui_finished:
            build();
            break;
        case BuilderInstancePrivate::fp_cui_error:
        {
            switch (d->fpCuiErrorCode)
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
                default:
                    emit builder_error(unknownError);
            }
            break;
        }
        case BuilderInstancePrivate::gcc_cui_finished:
            build();
            break;
        case BuilderInstancePrivate::gcc_cui_error:
        {
            switch (d->gccCuiErrorCode)
            {
                case GCCLauncher::ok:
                    emit builder_error(ok);
                    break;
                case GCCLauncher::exe_not_exists:
                    emit builder_error(exe_not_exists);
                    break;
                case GCCLauncher::unknownError:
                default:
                    emit builder_error(unknownError);
                    break;
            }
            d->timer.stop();
            break;
        }
        default:
            emit builder_error(unknownError);
            d->timer.stop();
    }
}

BuilderInstancePrivate::BuilderInstancePrivate(BuilderInstance* parent)
{
    this->q_ptr = parent;

    timer.setInterval(2000);
    buildStage = 0;

    connect(&compressor,
            SIGNAL(launcher_status_changed(CommandLauncher::launcher_status)),
            this,
            SLOT(onCompressorStatusChanged(CommandLauncher::launcher_status)));
    connect(&compressor,
            SIGNAL(compressorError(CompressorLauncher::ErrorCode)),
            this,
            SLOT(onCompressorError(CompressorLauncher::ErrorCode)));
    connect(&fp_cui,
            SIGNAL(launcher_status_changed(CommandLauncher::launcher_status)),
            this,
            SLOT(onFlatpakCuiStatusChanged(CommandLauncher::launcher_status)));
    connect(&fp_cui,
            SIGNAL(launcher_error(FlatpakLauncher::launcher_error_code)),
            this,
            SLOT(onFlatpakCuiError(FlatpakLauncher::launcher_error_code)));
    connect(&gcc_cui,
            SIGNAL(launcher_status_changed(CommandLauncher::launcher_status)),
            this,
            SLOT(onGCCCuiStatusChanged(CommandLauncher::launcher_status)));
    connect(&gcc_cui,
            SIGNAL(launcherError(GCCLauncher::ErrorCode)),
            this,
            SLOT(onGCCCuiError(GCCLauncher::ErrorCode)));
    connect(&timer,
            SIGNAL(timeout()),
            this,
            SLOT(onTimerTimeout()));
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

bool BuilderInstancePrivate::prepareSource()
{
    if (sourceList.count() < 1)
        return true;

    bool needToWait = false;
    for (int i=0; i<sourceList.count(); i++)
    {
        QString sourceURL;
        sourceURL = QString(sourceList[i].url).replace("file://", "");

        if (sourceList[i].type == FLATPAK_MANIFEST_SOURCE_TYPE_ARCHIVE &&
            sourceList[i].checksum.value.isEmpty())
        {
            // Do a minimum checksum test
            sourceList[i].checksum.algorithm = "md5";
            sourceList[i].checksum.value =
                ChecksumCalculator::getFileChecksum(sourceURL,
                                                    ChecksumCalculator::md5);
        }
        if (sourceList[i].type == FLATPAK_MANIFEST_SOURCE_TYPE_DIRECTORY)
        {
            // Archive directory to a single file
            QString archiveFileName;
            archiveFileName = QString("source_%1.tar")
                                     .arg(QString::number(i + 1))
                                     .prepend('/').prepend(workingDir);
            compressor.setWorkingDirectory(sourceURL);
            compressor.compress(".",
                                archiveFileName,
                                CompressorLauncher::TarFile);
            sourceList[i].url = archiveFileName.prepend("file://");
            sourceList[i].type = FLATPAK_MANIFEST_SOURCE_TYPE_ARCHIVE;
            needToWait = true;
        }
        if (needToWait)
            break;
    }
    if (needToWait)
        return false;

    // Initialize the module list in ManifestContainer during the first call
    if (manifest.getModules().count() < 1)
    {
        QMap<int,int>::const_iterator j;
        ManifestContainer::ModuleProperty module;
        for (int i=0; i<moduleList.count(); i++)
        {
            // Add sources to module
            module = moduleList[i];
            for (j=sourceIDs.constBegin(); j!=sourceIDs.constEnd(); j++)
            {
                if (j.value() == moduleIDs[i])
                    module.sources.push_back(
                                    sourceList[getSourceIndexByID(j.key())]);
            }
            manifest.addModules(module);
        }
    }
    return true;
}

bool BuilderInstancePrivate::buildManifest()
{
    if (moduleList.count() < 1 || sourceList.count() < 1)
        return false;

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

void BuilderInstancePrivate::buildExecutableHeader(bool preRun)
{
    Q_Q(BuilderInstance);

    // Check if the built bundle exists
    QFile bundleFile(bundlePath);
    if (!bundleFile.exists())
    {
        emit q->builder_error(BuilderInstance::bundle_not_exists);
        buildStage = 0;
        return;
    }

    // Extract source file for header compiling
    QFile tempFile;
    QFile resourceFile;
    QList<QString> fileList(QString(FPBDQT_BUILDER_INSTALLER_RESOURCES)
                                   .split('\n'));
    for (int i=0; i<fileList.count(); i++)
    {
        resourceFile.setFileName(fileList[i]);
        resourceFile.open(QFile::ReadOnly);

        fileList[i] = fileList[i].mid(fileList[i].lastIndexOf('/'))
                                 .prepend(workingDir);
        tempFile.setFileName(fileList[i]);
        tempFile.open(QFile::WriteOnly);
        tempFile.write(resourceFile.readAll());
        tempFile.close();

        resourceFile.close();
    }

    // Launch compiler to build a executable header for built bundle
    QString headerFileName = FPBDQT_BUILDER_INSTALLER_EXEC_HEADER;
    gcc_cui.setExecutablePath(FPBDQT_BUILDER_COMPILER_GCC_PATH);
    gcc_cui.setSourceFiles(QString(FPBDQT_BUILDER_INSTALLER_SOURCES)
                                  .split('\n'));
    if (!preRun)
    {
        QFile headerFile(QString(workingDir).append('/')
                                .append(headerFileName));
        if (!headerFile.exists())
        {
            emit q->builder_error(BuilderInstance::compiler_not_working);
            buildStage = 0;
            return;
        }
        gcc_cui.addDefinition("INSTALLER_HEADER_SIZE", headerFile.size());
    }
    gcc_cui.addDefinition("FLATPAK_BUNDLE_NAME", manifest.appID());
    gcc_cui.addDefinition("FLATPAK_BUNDLE_SIZE", bundleFile.size());
    gcc_cui.setWorkingDirectory(workingDir);
    gcc_cui.setStripAfterBuild(true);
    gcc_cui.compile(headerFileName);
}

void BuilderInstancePrivate::buildExecutable()
{
    Q_Q(BuilderInstance);

    // Check if the built header exists
    QString headerFileName = QString(workingDir).append('/')
                                .append(FPBDQT_BUILDER_INSTALLER_EXEC_HEADER);
    QFile headerFile(headerFileName);
    if (!headerFile.exists())
    {
        emit q->builder_error(BuilderInstance::compiler_not_working);
        buildStage = 0;
        return;
    }

    const int CopyBlockSize = 1024 * 1024;
    QFile bundleFile(bundlePath);
    bundleFile.open(QFile::ReadOnly);
    headerFile.open(QFile::Append);
    while (!bundleFile.atEnd())
    {
        headerFile.write(bundleFile.read(CopyBlockSize));
    }
    headerFile.close();
    bundleFile.close();

    QFile::remove(bundlePath);
    QFile::rename(headerFileName, bundlePath);
    q->build();
}

bool BuilderInstancePrivate::logCuiOutput(bool append)
{
    Q_Q(BuilderInstance);

    QFile log(logPath);
    if (append)
        log.open(QFile::Append);
    else
        log.open(QFile::WriteOnly);
    if (!log.isOpen())
        return false;

    int oldFileSize = log.size();
    log.write(compressor.output());
    log.write(fp_cui.output());
    log.write(gcc_cui.output());
    log.close();

    if (oldFileSize < log.size())
        emit q->output_updated();

    return true;
}

void BuilderInstancePrivate::onCompressorError(CompressorLauncher::ErrorCode errCode)
{
    compressorErrorCode = errCode;
    emit privateEvent(compressor_error);
}

void BuilderInstancePrivate::onCompressorStatusChanged(CommandLauncher::launcher_status status)
{
    if (status == CommandLauncher::finished)
        emit privateEvent(compressor_finished);
}

void BuilderInstancePrivate::onFlatpakCuiStatusChanged(
                                    CommandLauncher::launcher_status status)
{
    if (status == CommandLauncher::finished)
        emit privateEvent(fp_cui_finished);
}

void BuilderInstancePrivate::onFlatpakCuiError(
                                FlatpakLauncher::launcher_error_code errCode)
{
    fpCuiErrorCode = errCode;
    emit privateEvent(fp_cui_error);
}

void BuilderInstancePrivate::onGCCCuiError(GCCLauncher::ErrorCode errCode)
{
    gccCuiErrorCode = errCode;
    emit privateEvent(gcc_cui_error);
}

void BuilderInstancePrivate::onGCCCuiStatusChanged(
                                    CommandLauncher::launcher_status status)
{
    if (status == CommandLauncher::finished)
        emit privateEvent(gcc_cui_finished);
}

void BuilderInstancePrivate::onTimerTimeout()
{
    logCuiOutput();
}
