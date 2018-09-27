#ifndef GCCLAUNCHER_H
#define GCCLAUNCHER_H

#include "commandlauncher.h"


class GCCLauncherPrivate;

class GCCLauncher : public CommandLauncher
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(GCCLauncher)

public:
    enum ErrorCode
    {
        ok = 0,
        exe_not_exists = 129,
        compiler_still_running = 130,
        unknownError = 255,
    };
    enum OptimizationLevel
    {
        NoOptimization = 0,
        OptimizationLevel1 = 1,
        OptimizationLevel2 = 2,
        OptimizationLevel3 = 3,
        OptimizingSize = 4
    };

    GCCLauncher();
    bool compile(const QString& targetFile);

    QMap<QString, QVariant> definitions();
    void addDefinition(QString varName, const QVariant& varValue);
    void clearDefinitions();
    void removeDefinition(QString varName);

    QList<QString> sourceFiles();
    void addSourceFile(const QString& file);
    void clearSourceFiles();
    void removeSourceFile(const QString& file);
    void setSourceFiles(const QList<QString>& files);

    OptimizationLevel optimization();
    void setOptimization(OptimizationLevel level);

    bool stripAfterBuild();
    void setStripAfterBuild(bool strip);

    QString version();

signals:
    void launcherError(GCCLauncher::ErrorCode errCode);

protected slots:
    virtual void onPrivateEvent(int eventType);
};

#endif // GCCLAUNCHER_H
