#include "manifestcontainer.h"
#include "manifestcontainerprivate.h"
#include "manifest_format.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


QList<QString> ManifestContainer_jsonArrayToStrList(const QJsonArray& array);
QJsonArray ManifestContainer_strListToJsonArray(const QList<QString>& strList);

ManifestContainer::ManifestContainer()
{
    this->d_ptr = new ManifestContainerPrivate();
}

ManifestContainer::ParseErrorNumber
ManifestContainer::parse(const QByteArray content)
{
    const QJsonDocument& document = QJsonDocument::fromJson(content);
    if (document.isNull())
        return ContentInvalid;

    QJsonObject obj = document.object();

    // Parse application properties
    setAppID(obj[FLATPAK_MANIFEST_PROP_APPID].toString());
    setBranch(obj[FLATPAK_MANIFEST_PROP_BRANCH].toString());
    setRuntime(obj[FLATPAK_MANIFEST_PROP_RTNAME].toString());
    setRuntimeVer(obj[FLATPAK_MANIFEST_PROP_RTVER].toString());
    setSdkName(obj[FLATPAK_MANIFEST_PROP_SDKNAME].toString());
    setCommand(obj[FLATPAK_MANIFEST_PROP_COMMAND].toString());
    setSepLocale(obj[FLATPAK_MANIFEST_PROP_COMMAND].toBool());
    setTags(ManifestContainer_jsonArrayToStrList(
                obj[FLATPAK_MANIFEST_PROP_TAGS].toArray()));
    setFinishArgs(ManifestContainer_jsonArrayToStrList(
                      obj[FLATPAK_MANIFEST_PROP_FNARGS].toArray()));

    // Parse build options
    QJsonObject::const_iterator i;
    ManifestContainer::AppBuildOptions options;
    QJsonObject obj2 = obj[FLATPAK_MANIFEST_PROP_BUILDOPT]
                                    .toObject();
    if (obj.isEmpty())
        return NoModules;
    options.cflags = obj2[FLATPAK_MANIFEST_PROP_BUILD_CFLAG].toString();
    options.cxxflags = obj2[FLATPAK_MANIFEST_PROP_BUILD_CFLAG].toString();
    options.cppflags = obj2[FLATPAK_MANIFEST_PROP_BUILD_CFLAG].toString();
    options.prefix = obj2[FLATPAK_MANIFEST_PROP_BUILD_PREFIX].toString();
    options.configureArgs = ManifestContainer_jsonArrayToStrList(
                                    obj2[FLATPAK_MANIFEST_PROP_BUILD_CONFARG]
                                    .toArray());
    options.makeArgs = ManifestContainer_jsonArrayToStrList(
                                    obj2[FLATPAK_MANIFEST_PROP_BUILD_MAKEARG]
                                    .toArray());
    options.makeInstallArgs = ManifestContainer_jsonArrayToStrList(
                                    obj2[FLATPAK_MANIFEST_PROP_BUILD_MINSARG]
                                    .toArray());
    options.strip = obj2[FLATPAK_MANIFEST_PROP_BUILD_CFLAG].toBool();
    options.noDebugInfo = obj2[FLATPAK_MANIFEST_PROP_BUILD_CFLAG].toBool();
    setBuildOptions(options);

    // Parse module properties
    obj = obj[FLATPAK_MANIFEST_PROP_MODULES].toObject();
    if (obj.isEmpty())
        return NoModules;

    QJsonObject::const_iterator j;
    ManifestContainer::ModuleProperty module;
    ManifestContainer::ModuleSource source;
    for (i=obj.constBegin(); i!=obj.constEnd(); i++)
    {
        obj2 = (*i).toObject();
        if (obj2.isEmpty())
            continue;
        module.name = obj2[FLATPAK_MANIFEST_PROP_MODULES_NAME].toString();
        module.noAutoGen = obj2[FLATPAK_MANIFEST_PROP_MODULES_NAUTOGEN].toBool();
        module.noParaMake = obj2[FLATPAK_MANIFEST_PROP_MODULES_NPARAMK].toBool();
        module.noMakeInstall = obj2[FLATPAK_MANIFEST_PROP_MODULES_NMKINST].toBool();
        module.buildSystem = obj2[FLATPAK_MANIFEST_PROP_MODULES_BUILDSYS].toString();
        module.buildDir = obj2[FLATPAK_MANIFEST_PROP_MODULES_BUILDDIR].toString();
        module.subdir = obj2[FLATPAK_MANIFEST_PROP_MODULES_SUBDIR].toString();
        module.buildCommands = ManifestContainer_jsonArrayToStrList(
                                    obj2[FLATPAK_MANIFEST_PROP_MODULES_BUILDCMD]
                                    .toArray());
        module.installRule = obj2[FLATPAK_MANIFEST_PROP_MODULES_INSTRULE].toString();
        module.postInstall = ManifestContainer_jsonArrayToStrList(
                                obj2[FLATPAK_MANIFEST_PROP_MODULES_POSTINST]
                                .toArray());
        module.cleanUp = ManifestContainer_jsonArrayToStrList(
                                obj2[FLATPAK_MANIFEST_PROP_MODULES_CLEANUP]
                                .toArray());

        // Parse module sources
        if (obj2[FLATPAK_MANIFEST_PROP_MODULES_SOURCES].isNull())
            continue;
        obj2 = obj2[FLATPAK_MANIFEST_PROP_MODULES_SOURCES].toObject();

        for (j=obj2.constBegin(); j!=obj2.constEnd(); j++)
        {
            const QJsonObject& obj3 = (*j).toObject();
            if (obj3.isEmpty())
                continue;

            source.type = obj3[FLATPAK_MANIFEST_PROP_SOURCES_TYPE].toString();
            source.url = obj3[FLATPAK_MANIFEST_PROP_SOURCES_URL].toString();
            source.branch = obj3[FLATPAK_MANIFEST_PROP_SOURCES_BRANCH]
                                    .toString();
            source.commands = ManifestContainer_jsonArrayToStrList(
                                    obj3[FLATPAK_MANIFEST_PROP_SOURCES_CMDS]
                                    .toArray());
            source.destFilename = obj3[FLATPAK_MANIFEST_PROP_SOURCES_DESTFILE]
                                    .toString();
            module.sources.push_back(source);
        }
        addModules(module);
    }

    return OK;
}

QByteArray ManifestContainer::toJson()
{
    QJsonDocument document;
    QJsonObject obj;

    // Write application properties
    obj[FLATPAK_MANIFEST_PROP_APPID] = appID();
    obj[FLATPAK_MANIFEST_PROP_BRANCH] = branch();
    obj[FLATPAK_MANIFEST_PROP_RTNAME] = runtime();
    obj[FLATPAK_MANIFEST_PROP_RTVER] = runtimeVer();
    obj[FLATPAK_MANIFEST_PROP_SDKNAME] = sdkName();
    obj[FLATPAK_MANIFEST_PROP_COMMAND] = command();
    obj[FLATPAK_MANIFEST_PROP_SEPLCL] = sepLocale();
    obj[FLATPAK_MANIFEST_PROP_TAGS] =
                            ManifestContainer_strListToJsonArray(tags());
    obj[FLATPAK_MANIFEST_PROP_FNARGS] =
                            ManifestContainer_strListToJsonArray(finishArgs());

    // Write build options
    QJsonObject obj2;
    AppBuildOptions options = buildOptions();
    obj2[FLATPAK_MANIFEST_PROP_BUILD_CFLAG] = options.cflags ;
    obj2[FLATPAK_MANIFEST_PROP_BUILD_CXXFLAG] = options.cxxflags;
    obj2[FLATPAK_MANIFEST_PROP_BUILD_CPPFLAG] = options.cppflags;
    obj2[FLATPAK_MANIFEST_PROP_BUILD_PREFIX] = options.prefix;
    obj2[FLATPAK_MANIFEST_PROP_BUILD_CONFARG] =
                ManifestContainer_strListToJsonArray(options.configureArgs);
    obj2[FLATPAK_MANIFEST_PROP_BUILD_MAKEARG] =
                ManifestContainer_strListToJsonArray(options.makeArgs);
    obj2[FLATPAK_MANIFEST_PROP_BUILD_MINSARG] =
                ManifestContainer_strListToJsonArray(options.makeInstallArgs);
    obj2[FLATPAK_MANIFEST_PROP_BUILD_CFLAG] = options.strip;
    obj2[FLATPAK_MANIFEST_PROP_BUILD_CFLAG] = options.noDebugInfo;
    obj[FLATPAK_MANIFEST_PROP_BUILDOPT] = obj2;

    // Write module properties
    int i, j;
    obj2 = QJsonObject();
    QJsonObject obj3;
    QJsonArray moduleArray;
    QJsonArray sourceArray;
    QList<ModuleProperty> modules = getModules();
    for (i=0; i<modules.count(); i++)
    {
        moduleArray = QJsonArray();

        obj2[FLATPAK_MANIFEST_PROP_MODULES_NAME] = modules[i].name;
        obj2[FLATPAK_MANIFEST_PROP_MODULES_NAUTOGEN] = modules[i].noAutoGen;
        obj2[FLATPAK_MANIFEST_PROP_MODULES_NPARAMK] = modules[i].noParaMake;
        obj2[FLATPAK_MANIFEST_PROP_MODULES_NMKINST] = modules[i].noMakeInstall;
        obj2[FLATPAK_MANIFEST_PROP_MODULES_BUILDSYS] = modules[i].buildSystem;
        obj2[FLATPAK_MANIFEST_PROP_MODULES_BUILDCMD] =
                ManifestContainer_strListToJsonArray(modules[i].buildCommands);
        obj2[FLATPAK_MANIFEST_PROP_MODULES_INSTRULE] = modules[i].installRule;
        obj2[FLATPAK_MANIFEST_PROP_MODULES_POSTINST] =
                ManifestContainer_strListToJsonArray(modules[i].postInstall);
        obj2[FLATPAK_MANIFEST_PROP_MODULES_CLEANUP] =
                ManifestContainer_strListToJsonArray(modules[i].cleanUp);

        // Write module sources
        sourceArray = QJsonArray();
        for (j=0; j<modules[i].sources.count(); j++)
        {
            obj3[FLATPAK_MANIFEST_PROP_SOURCES_TYPE] =
                                            modules[i].sources[j].type;
            obj3[FLATPAK_MANIFEST_PROP_SOURCES_URL] =
                                            modules[i].sources[j].url;
            obj3[FLATPAK_MANIFEST_PROP_SOURCES_BRANCH] =
                                            modules[i].sources[j].branch;
            obj3[FLATPAK_MANIFEST_PROP_SOURCES_CMDS] =
                            ManifestContainer_strListToJsonArray(
                                                modules[i].sources[j].commands);
            obj3[FLATPAK_MANIFEST_PROP_SOURCES_DESTFILE] =
                                            modules[i].sources[j].destFilename;
            sourceArray.push_back(obj3);
        }
        obj2[FLATPAK_MANIFEST_PROP_MODULES_SOURCES] = sourceArray;

        moduleArray.push_back(obj2);
    }

    obj[FLATPAK_MANIFEST_PROP_MODULES] = moduleArray;

    document.setObject(obj);
    return document.toJson();
}

QString ManifestContainer::appID()
{
    Q_D(ManifestContainer);
    return d->appID;
}

void ManifestContainer::setAppID(const QString ID)
{
    Q_D(ManifestContainer);
    d->appID = ID;
}

QString ManifestContainer::branch()
{
    Q_D(ManifestContainer);
    return d->branchName;
}

void ManifestContainer::setBranch(const QString branch)
{
    Q_D(ManifestContainer);
    d->branchName = branch;
}

QString ManifestContainer::runtime()
{
    Q_D(ManifestContainer);
    return d->runtimeName;
}

void ManifestContainer::setRuntime(const QString runtime)
{
    Q_D(ManifestContainer);
    d->runtimeName = runtime;
}

QString ManifestContainer::runtimeVer()
{
    Q_D(ManifestContainer);
    return d->runtimeVer;
}

void ManifestContainer::setRuntimeVer(const QString runtimeVer)
{
    Q_D(ManifestContainer);
    d->runtimeVer = runtimeVer;
}

QString ManifestContainer::sdkName()
{
    Q_D(ManifestContainer);
    return d->runtimeName;
}

void ManifestContainer::setSdkName(const QString sdkName)
{
    Q_D(ManifestContainer);
    d->sdkName = sdkName;
}

QString ManifestContainer::command()
{
    Q_D(ManifestContainer);
    return d->command;
}

void ManifestContainer::setCommand(const QString command)
{
    Q_D(ManifestContainer);
    d->command = command;
}

bool ManifestContainer::sepLocale()
{
    Q_D(ManifestContainer);
    return d->isLocaleSeparated;
}

void ManifestContainer::setSepLocale(const bool sepLocale)
{
    Q_D(ManifestContainer);
    d->isLocaleSeparated = sepLocale;
}

ManifestContainer::AppBuildOptions ManifestContainer::buildOptions()
{
    Q_D(ManifestContainer);
    return d->buildOptions;
}

void ManifestContainer::setBuildOptions(const AppBuildOptions options)
{
    Q_D(ManifestContainer);
    d->buildOptions = options;
}

QList<QString> ManifestContainer::tags()
{
    Q_D(ManifestContainer);
    return d->tags;
}

void ManifestContainer::setTags(const QList<QString> tags)
{
    Q_D(ManifestContainer);
    d->tags = tags;
}

QList<QString> ManifestContainer::finishArgs()
{
    Q_D(ManifestContainer);
    return d->finishArgs;
}

void ManifestContainer::setFinishArgs(const QList<QString> finishArgs)
{
    Q_D(ManifestContainer);
    d->finishArgs = finishArgs;
}

void ManifestContainer::addModules(ModuleProperty module)
{
    Q_D(ManifestContainer);
    d->modules.push_back(module);
}

ManifestContainer::ModuleProperty
ManifestContainer::getModuleByName(QString name)
{
    Q_D(ManifestContainer);
    ModuleProperty module;
    for (int i=0; i<d->modules.count(); i++)
    {
        if (d->modules[i].name == name)
        {
            module = d->modules[i];
            break;
        }
    }
    return module;
}

QList<ManifestContainer::ModuleProperty> ManifestContainer::getModules()
{
    Q_D(ManifestContainer);
    return d->modules;
}

void ManifestContainer::setModuleByName(QString name,
                                        const ModuleProperty module)
{
    Q_D(ManifestContainer);
    for (int i=0; i<d->modules.count(); i++)
    {
        if (d->modules[i].name == name)
            d->modules[i] = module;
    }
}

void ManifestContainer::removeModuleByName(QString name)
{
    Q_D(ManifestContainer);
    for (int i=0; i<d->modules.count(); i++)
    {
        if (d->modules[i].name == name)
            d->modules.removeAt(i);
    }
}

QList<QString> ManifestContainer_jsonArrayToStrList(const QJsonArray& array)
{
    QList<QString> strList;
    for (int i=0; i<array.count(); i++)
        strList.push_back(array[i].toString());
    return strList;
}

QJsonArray ManifestContainer_strListToJsonArray(const QList<QString>& strList)
{
    QJsonArray array;
    for (int i=0; i<strList.count(); i++)
        array.push_back(strList[i]);
    return array;
}
