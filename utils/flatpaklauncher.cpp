#include "flatpaklauncher.h"
#include "flatpaklauncherprivate.h"

#define FLATPAK_BUILDER_OPTIONS_FORCECLEAN "--force-clean"
#define FLATPAK_BUILDER_OPTIONS_REPO "--repo"
#define FLATPAK_OPTIONS_BUILD_BUNDLE "build-bundle"


FlatpakLauncher::FlatpakLauncher()
{
    if (this->d_ptr)
        delete this->d_ptr;
    this->d_ptr = new FlatpakLauncherPrivate(this);
    connect(this->d_ptr,
            SIGNAL(privateEvent(int)),
            this,
            SLOT(onPrivateEvent(int)));
}

QString FlatpakLauncher::builderPath()
{
    Q_D(FlatpakLauncher);
    return d->fpbuilder_path;
}

void FlatpakLauncher::setBuilderPath(QString path)
{
    Q_D(FlatpakLauncher);
    d->fpbuilder_path = path;
}

QString FlatpakLauncher::flatpakPath()
{
    Q_D(FlatpakLauncher);
    return d->fp_path;
}

void FlatpakLauncher::setFlatpakPath(QString path)
{
    Q_D(FlatpakLauncher);
    d->fp_path = path;
}

QString FlatpakLauncher::appName()
{
    Q_D(FlatpakLauncher);
    return d->appName;
}

void FlatpakLauncher::setAppName(QString name)
{
    Q_D(FlatpakLauncher);
    d->appName = name;
}

QString FlatpakLauncher::repoDirectory()
{
    Q_D(FlatpakLauncher);
    return d->repoDirectory;
}

void FlatpakLauncher::setRepoDirectory(QString path)
{
    Q_D(FlatpakLauncher);
    d->repoDirectory = path;
}

QString FlatpakLauncher::repoBranch()
{
    Q_D(FlatpakLauncher);
    return d->repoBranch;
}

void FlatpakLauncher::setRepoBranch(QString branch)
{
    Q_D(FlatpakLauncher);
    d->repoBranch = branch;
}

bool FlatpakLauncher::build(QString manifest, QString buildDir)
{
    Q_D(FlatpakLauncher);

    if (!d->fileExists(d->fpbuilder_path))
    {
        emit launcher_error(exe_not_exists);
        return false;
    }

    QList<QString> args;
    args.append(FLATPAK_BUILDER_OPTIONS_FORCECLEAN);

    if (d->repoDirectory.isEmpty())
    {
        emit launcher_error(repo_not_exists);
        return false;
    }
    args.append(QString(FLATPAK_BUILDER_OPTIONS_REPO));
    args.append(d->repoDirectory);

    if (buildDir.isEmpty())
    {
        emit launcher_error(build_dir_not_exists);
        return false;
    }
    args.append(buildDir);

    if (manifest.isEmpty())
        return false;
    args.append(manifest);

    d->cui.setWorkingDirectory(d->workingDir);
    d->cui.start(d->fpbuilder_path,args);
    emit launcher_status_changed(running);

    d->arguments = args;
    return true;
}

bool FlatpakLauncher::buildBundle(QString targetFile)
{
    Q_D(FlatpakLauncher);

    if (!d->fileExists(d->fp_path))
    {
        emit launcher_error(exe_not_exists);
        return false;
    }

    QList<QString> args;
    args.append(FLATPAK_OPTIONS_BUILD_BUNDLE);

    if (d->repoDirectory.isEmpty())
    {
        emit launcher_error(repo_not_exists);
        return false;
    }
    args.append(d->repoDirectory);

    if (targetFile.isEmpty())
        return false;
    args.append(targetFile);

    if (d->appName.isEmpty())
    {
        emit launcher_error(app_name_not_exists);
        return false;
    }
    args.append(d->appName);

    d->cui.start(d->fp_path, args);
    emit launcher_status_changed(running);

    d->arguments = args;
    return true;
}

FlatpakLauncher::launcher_error_code FlatpakLauncher::errorCode()
{
    Q_D(FlatpakLauncher);
    return d->errorCode;
}

FlatpakLauncherPrivate::FlatpakLauncherPrivate(FlatpakLauncher *parent)
    : CommandLauncherPrivate(parent)
{
}
