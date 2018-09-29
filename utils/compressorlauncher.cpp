#include "compressorlauncher.h"
#include "compressorlauncherprivate.h"

#define COMPRESSOR_TAR_COMMAND_DEFAULT "tar"


CompressorLauncher::CompressorLauncher()
{
    if (this->d_ptr)
        delete this->d_ptr;
    this->d_ptr = new CompressorLauncherPrivate(this);
    connect(this->d_ptr,
            SIGNAL(privateEvent(int)),
            this,
            SLOT(onPrivateEvent(int)));
}

bool CompressorLauncher::compress(QString srcPath,
                                  QString destFileName,
                                  FileFormat format)
{
    Q_D(CompressorLauncher);
    if (d->cui_path.isEmpty())
        d->cui_path = COMPRESSOR_TAR_COMMAND_DEFAULT;

    QList<QString> args;
    switch (format)
    {
        case GzipFile:
            args.append("-Z");
            break;
        case BzipFile:
            args.append("-j");
            break;
        case xzFile:
            args.append("-J");
            break;
        case TarFile:
        default:;
    }
    args.append("-c");
    args.append(QString("-f").append(destFileName));
    args.append(srcPath);

    d->cui.setWorkingDirectory(d->workingDir);
    d->cui.start(d->cui_path, args);

    return true;
}

CompressorLauncherPrivate::CompressorLauncherPrivate(CompressorLauncher *parent)
    : CommandLauncherPrivate(parent)
{
}
