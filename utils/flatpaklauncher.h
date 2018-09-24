#ifndef FLATPAKLAUNCHER_H
#define FLATPAKLAUNCHER_H

#include "commandlauncher.h"


class FlatpakLauncherPrivate;

class FlatpakLauncher : public CommandLauncher
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FlatpakLauncher)

public:
    enum launcher_error_code
    {
        ok = 0,
        manifest_not_exists = 1,
        app_name_not_exists = 2,
        repo_not_exists = 3,
        build_dir_not_exists = 4,
        exe_not_exists = 129,
        no_permission = 130,
        unknownError = 255,
    };

    FlatpakLauncher();
    QString builderPath();
    void setBuilderPath(QString path);
    QString flatpakPath();
    void setFlatpakPath(QString path);
    QString appName();
    void setAppName(QString name);
    QString repoDirectory();
    void setRepoDirectory(QString path);
    QString repoBranch();
    void setRepoBranch(QString branch);
    bool build(QString manifest, QString buildDir);
    bool buildBundle(QString targetFile);
    launcher_error_code errorCode();

signals:
    void launcher_error(FlatpakLauncher::launcher_error_code errCode);
};

#endif // FLATPAKLAUNCHER_H
