#ifndef MANIFESTCONTAINER_H
#define MANIFESTCONTAINER_H

#include <QObject>
#include <QList>


class ManifestContainerPrivate;

class ManifestContainer : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ManifestContainer)

public:
    enum ParseErrorNumber
    {
        OK = 0,
        ContentInvalid = 1,
        NoModules = 2,
    };

    struct AppBuildOptions
    {
        QString cflags;
        QString cxxflags;
        QString cppflags;
        QString ldflags;
        QString prefix;
        QList<QString> configureArgs;
        QList<QString> makeArgs;
        QList<QString> makeInstallArgs;
        bool strip;
        bool noDebugInfo;
    };

    struct ModuleSource
    {
        QString type;
        QString url;
        QString branch;
        QList<QString> commands;
        QString destFilename;
    };

    struct ModuleProperty
    {
        QString name;
        bool noAutoGen;
        bool noParaMake;
        bool noMakeInstall;
        QString buildSystem;
        QString buildDir;
        QString subdir;
        QList<QString> buildCommands;
        QString installRule;
        QList<QString> postInstall;
        QList<QString> cleanUp;
        QList<ModuleSource> sources;
    };

    ManifestContainer();

    ParseErrorNumber parse(const QByteArray content);
    QByteArray toJson();

    Q_PROPERTY(QString appID READ appID WRITE setAppID)
    Q_PROPERTY(QString branch READ branch WRITE setBranch)
    Q_PROPERTY(QString runtime READ runtime WRITE setRuntime)
    Q_PROPERTY(QString runtimeVer READ runtimeVer WRITE setRuntimeVer)
    Q_PROPERTY(QString sdkName READ sdkName WRITE setSdkName)
    Q_PROPERTY(QString command READ command WRITE setCommand)
    Q_PROPERTY(bool sepLocale READ sepLocale WRITE setSepLocale)
    Q_PROPERTY(QList<QString> tags READ tags WRITE setTags)
    Q_PROPERTY(AppBuildOptions buildOptions READ buildOptions
                                            WRITE setBuildOptions)
    Q_PROPERTY(QList<QString> finishArgs READ finishArgs WRITE setFinishArgs)

    QString appID();
    void setAppID(const QString ID);
    QString branch();
    void setBranch(const QString branch);
    QString runtime();
    void setRuntime(const QString runtime);
    QString runtimeVer();
    void setRuntimeVer(const QString runtimeVer);
    QString sdkName();
    void setSdkName(const QString sdkName);
    QString command();
    void setCommand(const QString command);
    bool sepLocale();
    void setSepLocale(const bool sepLocale);
    AppBuildOptions buildOptions();
    void setBuildOptions(const AppBuildOptions options);
    QList<QString> tags();
    void setTags(const QList<QString> tags);
    QList<QString> finishArgs();
    void setFinishArgs(const QList<QString> finishArgs);

    void addModules(ModuleProperty module);
    QList<ModuleProperty> getModules();
    ModuleProperty getModuleByName(QString name);
    void setModuleByName(QString name, const ModuleProperty module);
    void removeModuleByName(QString name);
};

#endif // MANIFESTCONTAINER_H
