#ifndef COMPRESSORLAUNCHER_H
#define COMPRESSORLAUNCHER_H

#include "commandlauncher.h"


class CompressorLauncherPrivate;

class CompressorLauncher : public CommandLauncher
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CompressorLauncher)

public:
    enum FileFormat
    {
        TarFile = 0,
        GzipFile = 1,
        BzipFile = 2,
        xzFile = 3
    };
    enum ErrorCode
    {
        ok = 0,
        command_not_exists = 1,
        source_not_exists = 2,
        compressor_still_running = 129,
        unknownError = 255,
    };

    CompressorLauncher();
    bool compress(QString srcPath, QString destFileName, FileFormat format);

signals:
    void compressorError(CompressorLauncher::ErrorCode errCode);
};

#endif // COMPRESSORLAUNCHER_H
