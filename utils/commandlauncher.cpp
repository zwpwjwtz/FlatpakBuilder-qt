#include "commandlauncher.h"
#include "commandlauncherprivate.h"

#include <QFileInfo>


CommandLauncher::CommandLauncher()
{
    this->d_ptr = new CommandLauncherPrivate(this);
    connect(this->d_ptr,
            SIGNAL(privateEvent(int)),
            this,
            SLOT(onPrivateEvent(int)));
}

QString CommandLauncher::executablePath()
{
    Q_D(CommandLauncher);
    return d->cui_path;
}

bool CommandLauncher::setExecutablePath(QString path)
{
    Q_D(CommandLauncher);
    if (d->fileExists(path))
    {
        d->cui_path = path;
        return true;
    }
    else
        return false;
}

QList<QString> CommandLauncher::arguments()
{
    Q_D(CommandLauncher);
    return d->arguments;
}

void CommandLauncher::setArguments(const QList<QString>& args)
{
    Q_D(CommandLauncher);
    d->arguments = args;
}

QString CommandLauncher::workingDirectory()
{
    Q_D(CommandLauncher);
    return d->workingDir;
}

void CommandLauncher::setWorkingDirectory(QString dir)
{
    Q_D(CommandLauncher);
    d->workingDir = dir;
}

bool CommandLauncher::run(bool synchronized)
{
    Q_D(CommandLauncher);
    d->cui.setWorkingDirectory(d->workingDir);
    d->cui.start(d->cui_path, d->arguments);

    if (synchronized)
    {
        if (!d->cui.waitForStarted())
            return false;
        return d->cui.waitForFinished(-1);
    }
    else
        return true;
}

int CommandLauncher::exitCode()
{
    Q_D(CommandLauncher);
    return d->cui.exitCode();
}

QByteArray CommandLauncher::output()
{
    Q_D(CommandLauncher);
    return d->cui.readAllStandardOutput()
                        .append(d->cui.readAllStandardError());
}

void CommandLauncher::onPrivateEvent(int eventType)
{
    switch (eventType) {
        case CommandLauncherPrivate::cui_finished:
            emit launcher_status_changed(finished);
            break;
        default:
            break;
    }
}

CommandLauncherPrivate::CommandLauncherPrivate(CommandLauncher* parent)
{
    this->q_ptr = parent;
    connect(&cui,
            SIGNAL(finished(int, QProcess::ExitStatus)),
            this,
            SLOT(onCuiFinished()));
}

bool CommandLauncherPrivate::fileExists(QString path)
{
    QFileInfo file(path);
    return file.exists();
}

void CommandLauncherPrivate::onCuiFinished()
{
    emit privateEvent(cui_finished);
}
