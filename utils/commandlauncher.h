#ifndef COMMANDLAUNCHER_H
#define COMMANDLAUNCHER_H

#include <QObject>


class CommandLauncherPrivate;

class CommandLauncher : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CommandLauncher)
protected:
    CommandLauncherPrivate* d_ptr;

public:
    enum launcher_status
    {
        ready = 0,
        running = 1,
        finished = 2,
        stopped = 3,
    };

    explicit CommandLauncher();

    virtual QString executablePath();
    virtual bool setExecutablePath(QString path);
    virtual QList<QString> arguments();
    virtual void setArguments(const QList<QString>& args);
    virtual QString workingDirectory();
    virtual void setWorkingDirectory(QString dir);

    virtual bool run(bool synchronized = false);
    virtual int exitCode();
    virtual QByteArray output();

signals:
    void launcher_status_changed(CommandLauncher::launcher_status status);

private slots:
    virtual void onPrivateEvent(int eventType);
};

#endif // COMMANDLAUNCHER_H
