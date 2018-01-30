#include "exeprober.h"
#include "exeproberprivate.h"

#include <QFileInfo>


ExeProber::ExeProber()
{
    this->d_ptr = new ExeProberPrivate;
}

ExeProber::probe_error_code
ExeProber::probeExe(QString path)
{
    Q_D(ExeProber);

    if (!d->fileExists(path))
        return probe_file_not_exists;

    d->process.setArguments(d->arguments);
    d->process.start(path);
    d->arguments.clear();

    switch (d->process.error())
    {
        case QProcess::FailedToStart:
        case QProcess::Timedout:
            return probe_cannot_start;
        case QProcess::Crashed:
            return probe_process_stopped;
        case QProcess::ReadError:
        case QProcess::WriteError:
            return probe_unknown_error;
        default:;
    }

    if (d->process.exitStatus() == QProcess::NormalExit)
        return probe_ok;
    else
        return probe_process_stopped;
}

void ExeProber::setArguments(QList<QString> args)
{
    Q_D(ExeProber);
    d->arguments = args;
}

int ExeProber::returnValue()
{
    Q_D(ExeProber);
    return d->process.exitCode();
}

QByteArray ExeProber::output()
{
    Q_D(ExeProber);
    return d->process.readAll();
}

ExeProberPrivate::ExeProberPrivate()
{

}

bool ExeProberPrivate::fileExists(QString path)
{
    QFileInfo file(path);
    return file.exists();
}
