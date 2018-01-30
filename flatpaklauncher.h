#ifndef FLATPAKLAUNCHER_H
#define FLATPAKLAUNCHER_H

#include <QObject>
#include <QList>
#include <QString>

class FlatpakLauncherPrivate;

class FlatpakLauncher : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FlatpakLauncher)

    enum launcher_status
    {
        ready = 0,
        running = 1,
        finished = 2,
        stopped = 3,
    };

    enum launcher_error_code
    {
        ok = 0,
        manifest_not_exists = 1,
        app_name_not_exists = 2,
        repo_not_exists = 3,
        exe_not_exists = 129,
        no_permission = 130,
        unknownError = 255,
    };

public:
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
    QList<QString> arguments();
    bool build(QString manifest, QString buildDir);
    bool buildBundle(QString targetFile);
    int exitCode();
    launcher_error_code errorCode();
    QByteArray output();

signals:
    void launcher_status_changed(launcher_status status);
    void launcher_error(launcher_error_code errCode);

private slots:
    void onPrivateEvent(int eventType);

protected:
    FlatpakLauncherPrivate* d_ptr;
};

#endif // FLATPAKLAUNCHER_H
