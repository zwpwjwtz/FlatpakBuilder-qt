#ifndef FLATPAKLAUNCHERPRIVATE_H
#define FLATPAKLAUNCHERPRIVATE_H

#include <QProcess>

#include "flatpaklauncher.h"

class FlatpakLauncher;

class FlatpakLauncherPrivate : QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(FlatpakLauncher)


public:
    enum private_event_type
    {
        none = 0,
        cui_finished = 1
    };

    FlatpakLauncherPrivate();
    bool fileExists(QString path);

    QProcess cui;
    QString fp_path;
    QString fpbuilder_path;
    QString appName;
    QString repoDirectory;
    QString repoBranch;
    QString workingDir;
    QList<QString> arguments;
    FlatpakLauncher::launcher_error_code errorCode;

signals:
    void privateEvent(int eventType);

private slots:
    void onCuiFinished();

protected:
    FlatpakLauncher* q_ptr;
};

#endif // FLATPAKLAUNCHERPRIVATE_H
