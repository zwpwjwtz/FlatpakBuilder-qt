#ifndef COMMANDLAUNCHERPRIVATE_H
#define COMMANDLAUNCHERPRIVATE_H

#include <QProcess>


class CommandLauncher;

class CommandLauncherPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CommandLauncher)
protected:
    CommandLauncher* q_ptr;

public:
    enum private_event_type
    {
        none = 0,
        cui_finished = 1
    };

    CommandLauncherPrivate(CommandLauncher* parent);
    bool fileExists(QString path);

    QProcess cui;
    QString cui_path;
    QString workingDir;
    QList<QString> arguments;

signals:
    void privateEvent(int eventType);

private slots:
    void onCuiFinished();
};
#endif // COMMANDLAUNCHERPRIVATE_H
