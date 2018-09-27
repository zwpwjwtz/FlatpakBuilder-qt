#include <QMap>
#include <QVariant>
#include "gcclauncher.h"
#include "gcclauncherprivate.h"

#define GCCLAUNCHER_GCC_COMMAND_DEFAULT "gcc"
#define GCCLAUNCHER_STRIP_COMMAND_DEFAULT "strip"

#define GCCLAUNCHER_GCC_OPTION_VERSION "--version"
#define GCCLAUNCHER_GCC_OPTION_OPTIMIZATION "-O"
#define GCCLAUNCHER_GCC_OPTION_OUTPUT "-o"


GCCLauncher::GCCLauncher()
{
    if (this->d_ptr)
        delete this->d_ptr;
    this->d_ptr = new GCCLauncherPrivate(this);
    connect(this->d_ptr,
            SIGNAL(privateEvent(int)),
            this,
            SLOT(onPrivateEvent(int)));
}

QMap<QString, QVariant> GCCLauncher::definitions()
{
    Q_D(GCCLauncher);
    return d->definitions;
}

void GCCLauncher::addDefinition(QString varName, const QVariant& varValue)
{
    Q_D(GCCLauncher);
    d->definitions.insert(varName, varValue);
}

void GCCLauncher::clearDefinitions()
{
    Q_D(GCCLauncher);
    d->definitions.clear();
}

void GCCLauncher::removeDefinition(QString varName)
{
    Q_D(GCCLauncher);
    d->definitions.remove(varName);
}

QList<QString> GCCLauncher::sourceFiles()
{
    Q_D(GCCLauncher);
    return d->sourceFiles;
}

void GCCLauncher::addSourceFile(const QString& file)
{
    Q_D(GCCLauncher);
    if (!d->sourceFiles.contains(file))
        d->sourceFiles.push_back(file);
}

void GCCLauncher::clearSourceFiles()
{
    Q_D(GCCLauncher);
    d->sourceFiles.clear();
}

void GCCLauncher::removeSourceFile(const QString& file)
{
    Q_D(GCCLauncher);
    d->sourceFiles.removeAll(file);
}

void GCCLauncher::setSourceFiles(const QList<QString>& files)
{
    Q_D(GCCLauncher);
    d->sourceFiles = files;
}

GCCLauncher::OptimizationLevel GCCLauncher::optimization()
{
    Q_D(GCCLauncher);
    return d->optimizingLevel;
}

void GCCLauncher::setOptimization(GCCLauncher::OptimizationLevel level)
{
    Q_D(GCCLauncher);
    d->optimizingLevel = level;
}

bool GCCLauncher::stripAfterBuild()
{
    Q_D(GCCLauncher);
    return d->stripAfterBuild;
}

void GCCLauncher::setStripAfterBuild(bool strip)
{
    Q_D(GCCLauncher);
    d->stripAfterBuild = strip;
}

QString GCCLauncher::version()
{
    Q_D(GCCLauncher);
    QString verionsString;

    if (d->cui_path.isEmpty())
        return verionsString;

    QList<QString> args;
    args.append(GCCLAUNCHER_GCC_OPTION_VERSION);
    d->cui.execute(d->cui_path, args);
    if (d->cui.exitCode() != 0)
        return verionsString;

    // Assuming the first line of the output containing
    // a version string at end
    QList<QByteArray> outputLines(output().split('\n'));
    if (outputLines.count() > 0)
        verionsString = outputLines[0].split(' ').last();
    return verionsString.trimmed();
}

bool GCCLauncher::compile(const QString& targetFile)
{
    Q_D(GCCLauncher);
    if (d->cui_path.isEmpty() || d->sourceFiles.isEmpty())
    {
        emit launcherError(exe_not_exists);
        return false;
    }

    // Terminate the compiler if it is still running
    if (d->isCompiling)
    {
        d->cui.terminate();
        d->cui.waitForFinished();
    }
    if (d->cui.state() != QProcess::NotRunning)
    {
        emit launcherError(compiler_still_running);
        return false;
    }

    QList<QString> args;
    args.push_back(d->optimizationLevelToString(d->optimizingLevel));
    args.push_back(QString(GCCLAUNCHER_GCC_OPTION_OUTPUT).append(targetFile));

    // Append constant definitions
    QString tempArg;
    QMap<QString, QVariant>::const_iterator i;
    for (i=d->definitions.begin(); i!=d->definitions.end(); i++)
    {
        tempArg = QString("-D%1=").arg(i.key());
        if (i.value().type() == QVariant::String ||
            i.value().type() == QVariant::Char)
            tempArg.append("\"").append(i.value().toString()).append("\"");
        else
            tempArg.append(QString::number(i.value().toLongLong()));
        args.append(tempArg);
    }

    // Append user-defined arguments
    if (!d->arguments.isEmpty())
    {
        args.append(d->arguments);
    }
    args.append(d->sourceFiles);

    // Launching the compiler
    d->isCompiling = true;
    d->targetFile = targetFile;
    d->cui.setWorkingDirectory(d->workingDir);
    d->cui.start(d->cui_path, args);
    return true;
}

void GCCLauncher::onPrivateEvent(int eventType)
{
    Q_D(GCCLauncher);
    switch (eventType)
    {
        case CommandLauncherPrivate::cui_finished:
            if (d->isCompiling)
            {
                d->isCompiling = false;
                if (d->stripAfterBuild)
                    d->stripTargetFile();
                emit launcher_status_changed(finished);
            }
            break;
        default:
            break;
    }
}

GCCLauncherPrivate::GCCLauncherPrivate(GCCLauncher* parent)
    : CommandLauncherPrivate(parent)
{
    isCompiling = false;
    optimizingLevel = OLevel::OptimizationLevel2;
}

bool GCCLauncherPrivate::stripTargetFile()
{
    int exitCode = cui.execute(QString(GCCLAUNCHER_STRIP_COMMAND_DEFAULT)
                                      .append(" ").append(workingDir)
                                      .append('/').append(targetFile));
    return (exitCode == 0);
}

QString GCCLauncherPrivate::optimizationLevelToString(OLevel level)
{
    QString levelString(GCCLAUNCHER_GCC_OPTION_OPTIMIZATION);
    switch (level)
    {
        case OLevel::NoOptimization:
            levelString.append('0');
            break;
        case OLevel::OptimizationLevel1:
            levelString.append('1');
            break;

        case OLevel::OptimizationLevel2:
            levelString.append('2');
            break;

        case OLevel::OptimizationLevel3:
            levelString.append('3');
            break;

        case OLevel::OptimizingSize:
            levelString.append('s');
            break;
        default:;
    }
    return levelString;
}
