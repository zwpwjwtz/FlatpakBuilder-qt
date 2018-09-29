#ifndef COMPRESSORLAUNCHERPRIVATE_H
#define COMPRESSORLAUNCHERPRIVATE_H

#include "commandlauncherprivate.h"


class CompressorLauncher;

class CompressorLauncherPrivate : public CommandLauncherPrivate
{
    Q_DECLARE_PUBLIC(CommandLauncher)

public:
    CompressorLauncherPrivate(CompressorLauncher* parent);
};

#endif // COMPRESSORLAUNCHERPRIVATE_H
