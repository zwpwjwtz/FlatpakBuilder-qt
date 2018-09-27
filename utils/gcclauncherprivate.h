#ifndef GCCLAUNCHERPRIVATE_H
#define GCCLAUNCHERPRIVATE_H

#include "commandlauncherprivate.h"
#include "gcclauncher.h"


class GCCLauncher;

class GCCLauncherPrivate : public CommandLauncherPrivate
{
    Q_DECLARE_PUBLIC(GCCLauncher)

public:
    typedef GCCLauncher::OptimizationLevel OLevel;

    OLevel optimizingLevel;
    QMap<QString, QVariant> definitions;
    QList<QString> sourceFiles;
    QString targetFile;
    bool isCompiling;
    bool stripAfterBuild;

    GCCLauncherPrivate(GCCLauncher* parent);
    bool stripTargetFile();
    static QString optimizationLevelToString(OLevel level);
};

#endif // GCCLAUNCHERPRIVATE_H
