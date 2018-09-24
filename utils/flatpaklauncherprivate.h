#ifndef FLATPAKLAUNCHERPRIVATE_H
#define FLATPAKLAUNCHERPRIVATE_H

#include "flatpaklauncher.h"
#include "commandlauncherprivate.h"


class FlatpakLauncher;

class FlatpakLauncherPrivate : public CommandLauncherPrivate
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(FlatpakLauncher)

public:
    FlatpakLauncherPrivate(FlatpakLauncher* parent);

    QString fp_path;
    QString fpbuilder_path;
    QString appName;
    QString repoDirectory;
    QString repoBranch;
    FlatpakLauncher::launcher_error_code errorCode;
};

#endif // FLATPAKLAUNCHERPRIVATE_H
