#include "buildwizard.h"
#include "ui_buildwizard.h"
#include "exeprober.h"
#include "manifestcontainer.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

#define FPBDQT_COMMAND_FLATPAK "/usr/bin/flatpak"
#define FPBDQT_COMMAND_FPBUILDER "/usr/bin/flatpak-builder"
#define FPBDQT_COMMAND_FPBUILDER_PROBEARGS "--version"
#define FPBDQT_COMMAND_QMAKE "/usr/bin/qmake"
#define FPBDQT_COMMAND_QMAKE_PROBEARGS "--version"

#define FPBDQT_LIBRARY_FLATPAK_RUNTIME "/var/lib/flatpak/runtime"

#define FPBDQT_FILE_SUFFIX_ALL "All (*.*)(*.*)"
#define FPBDQT_FILE_SUFFIX_PRO "Qt Project File (*.pro)(*.pro)"
#define FPBDQT_FILE_LOG_DEFAULT "build_log.txt"

#define FPBDQT_APP_PREFIX_DEFAULT "com.example."
#define FPBDQT_APP_MANIFEST_DEFAULT "manifest.json"
#define FPBDQT_APP_BUILDDIR_DEFAULT "build"
#define FPBDQT_APP_REPO_DEFAULT "repo"
#define FPBDQT_APP_BRANCH_DEFAULT "master"
#define FPBDQT_APP_FINARGS_SHARE_IPC "--share=ipc"
#define FPBDQT_APP_FINARGS_SHARE_NET "--share=network"
#define FPBDQT_APP_FINARGS_SOCKET_X11 "--socket=x11"
#define FPBDQT_APP_FINARGS_DEVICE_DRI "--device=dri"
#define FPBDQT_APP_FINARGS_FS_HOST "--filesystem=host"

#define FPBDQT_SOURCE_BRANCH_DEFAULT "master"


BuildWizard::BuildWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::BuildWizard)
{
    ui->setupUi(this);

    connect(&fp_cui,
            SIGNAL(launcher_status_changed(FlatpakLauncher::launcher_status)),
            this,
            SLOT(onCuiStatusChanged(FlatpakLauncher::launcher_status)));

    buildStage = 0;
}

BuildWizard::~BuildWizard()
{
    delete ui;
}

QString BuildWizard::getDefaultProjectName(QString fileName)
{
    QFileInfo proFile(fileName);
    QString projectName = proFile.fileName();
    projectName.truncate(projectName.indexOf(QChar('.')));
    return projectName;
}

bool BuildWizard::detectEssentials()
{
    ExeProber prober;
    QList<QString> args;
    ExeProber::probe_error_code errorCode;

    // Check for flatpak-builder
    args.append(FPBDQT_COMMAND_FPBUILDER_PROBEARGS);
    prober.setArguments(args);
    errorCode = prober.probeExe(FPBDQT_COMMAND_FPBUILDER);
    if (errorCode == ExeProber::probe_file_not_exists)
    {
        QMessageBox::critical(this,
                              "Missing component",
                              "Flatpak-builder isn't detected. "
                              "Please install flatpak-builder and try again.");
        return false;
    }
    else if (errorCode != ExeProber::probe_ok)
    {
        QMessageBox::critical(this,
                              "Component error",
                              "Flatpak-builder cannot be started. "
                              "Please check if you have the right permission "
                              "to run flatpak-builder.");
        return false;
    }
    fp_cui.setBuilderPath(FPBDQT_COMMAND_FPBUILDER);
    fp_cui.setFlatpakPath(FPBDQT_COMMAND_FLATPAK);

    // Check for qmake
    args.clear();
    args.append(FPBDQT_COMMAND_QMAKE_PROBEARGS);
    prober.setArguments(args);
    errorCode = prober.probeExe(FPBDQT_COMMAND_QMAKE);
    if (errorCode == ExeProber::probe_file_not_exists)
    {
        QMessageBox::critical(this,
                              "Missing component",
                              "Qmake isn't detected. "
                              "Please install qmake and try again.");
        return false;
    }
    else if (errorCode != ExeProber::probe_ok)
    {
        QMessageBox::critical(this,
                              "Component error",
                              "Qmake cannot be started. "
                              "Please check if you have the right permission "
                              "to run qmake.");
        return false;
    }

    return true;

}

bool BuildWizard::detectRuntime(QString &rtName, QString &rtVersion)
{
    // Read runtime directory and get the first entry
    QString dirName = findFirstDirEntry(FPBDQT_LIBRARY_FLATPAK_RUNTIME);
    if (dirName.isEmpty())
        return false;
    rtName = dirName;

    // Read the first entry of the entry we have found
    // It should be a directory having name like "x86_64"
    dirName = QString(FPBDQT_LIBRARY_FLATPAK_RUNTIME)
            .append('/').append(rtName);
    dirName = findFirstDirEntry(dirName);
    if (dirName.isEmpty())
        return false;

    // Read the first entry in the third level directory
    // It should be a directory having a version-like name
    dirName = QString(FPBDQT_LIBRARY_FLATPAK_RUNTIME)
            .append('/').append(rtName)
            .append('/').append(dirName);
    dirName = findFirstDirEntry(dirName);
    if (dirName.isEmpty())
        return false;
    rtVersion = dirName;

    return true;
}

bool BuildWizard::detectSdk(QString &sdkName)
{
    // Simply get name from related runtime
    QString version;
    detectRuntime(sdkName, version);
    if (sdkName.isEmpty())
        return false;

    sdkName.truncate(sdkName.lastIndexOf('.'));
    sdkName.append(".Sdk");

    return true;
}

QString BuildWizard::findFirstDirEntry(QString path)
{
    QString found;

    QDir dir(path);
    QFileInfo fileInfo;
    for (int i=0; i<dir.entryList().count(); i++)
    {
        if (dir.entryList()[i] == "." || dir.entryList()[i] == "..")
            continue;
        fileInfo.setFile(dir, dir.entryList()[i]);
        if (fileInfo.isReadable() && fileInfo.isDir())
        {
            found = dir.entryList()[i];
            break;
        }
    }
    return found;
}

bool BuildWizard::logCuiOutput(QString filePath, bool append)
{
    QFile log(filePath);
    if (append)
        log.open(QFile::Append);
    else
        log.open(QFile::WriteOnly);
    if (!log.isOpen())
        return false;

    log.write(fp_cui.output());
    log.close();
    return true;
}

void BuildWizard::startBuild()
{
    if (buildStage == 0)
    {
        // Create a manifest
        ManifestContainer manifest;
        ManifestContainer::ModuleProperty module;
        ManifestContainer::ModuleSource source, source2;
        manifest.setAppID(ui->textAppID->text());
        manifest.setBranch(FPBDQT_APP_BRANCH_DEFAULT);
        manifest.setRuntime(ui->textRuntime->text());
        manifest.setSdkName(ui->textSDK->text());
        manifest.setRuntimeVer(ui->textRuntimeVer->text());
        manifest.setCommand(appName);
        QList<QString> finishArgs;
        finishArgs.push_back(FPBDQT_APP_FINARGS_DEVICE_DRI);
        finishArgs.push_back(FPBDQT_APP_FINARGS_FS_HOST);
        finishArgs.push_back(FPBDQT_APP_FINARGS_SHARE_IPC);
        finishArgs.push_back(FPBDQT_APP_FINARGS_SHARE_NET);
        finishArgs.push_back(FPBDQT_APP_FINARGS_SOCKET_X11);
        manifest.setFinishArgs(finishArgs);

        // Setup the main module and its sources
        module.name = appName;
        source.type = "git";
        source.url = ui->labelSrcDir->text().prepend("file://");
        source.branch = FPBDQT_SOURCE_BRANCH_DEFAULT;
        source2.type = "script";
        source2.commands.append("qmake PREFIX=/app");
        source2.destFilename = "configure";
        module.sources.append(source);
        module.sources.append(source2);
        manifest.addModules(module);

        // Write the manifest to file
        QString manifest_path;
        manifest_path = ui->textBuildPath->text()
                .append('/')
                .append(FPBDQT_APP_MANIFEST_DEFAULT);
        QFile jsonFile(manifest_path);
        jsonFile.open(QFile::WriteOnly);
        jsonFile.write(manifest.toJson());
        jsonFile.close();

        // Launch flatpak-builder
        fp_cui.setRepoDirectory(ui->textBuildPath->text()
                                .append('/')
                                .append(FPBDQT_APP_REPO_DEFAULT));
        buildStage = 1;
        fp_cui.build(manifest_path,
                     ui->textBuildPath->text().
                     append('/')
                     .append(FPBDQT_APP_BUILDDIR_DEFAULT));
    }
    else if (buildStage == 1)
    {
        // Write log generated in last stage
        logCuiOutput(ui->textBuildPath->text()
                     .append('/')
                     .append(FPBDQT_FILE_LOG_DEFAULT), false);

        // Launch flatpak to build a bundle file
        fp_cui.setAppName(ui->textAppID->text());
        buildStage = 2;
        fp_cui.buildBundle(ui->labelTargetFile->text());
    }
    else
    {
        // Write log generated in last stage
        logCuiOutput(ui->textBuildPath->text()
                     .append('/')
                     .append(FPBDQT_FILE_LOG_DEFAULT), true);

        // Finish
        buildStage = 3;
        next();
    }
}

void BuildWizard::on_pushButton_clicked()
{
    if (lastFileFilter.isEmpty())
        lastFileFilter = FPBDQT_FILE_SUFFIX_PRO;

    QString path = QFileDialog::getOpenFileName(this,
                     "Open Qt Project File",
                     lastSrcPath,
                     QString(FPBDQT_FILE_SUFFIX_ALL).append(";;")
                     .append(FPBDQT_FILE_SUFFIX_PRO).append(";;"),
                     &lastFileFilter);
    if (path.isEmpty())
        return;
    lastSrcPath = path;
    ui->textProPath->setText(path);

    QFileInfo proFile(path);
    ui->labelSrcDir->setText(proFile.dir().path());

    appName = proFile.fileName();
    appName.truncate(appName.indexOf('.'));
    ui->textAppID->setText(QString(FPBDQT_APP_PREFIX_DEFAULT).append(appName));
}

void BuildWizard::on_pushButton_2_clicked()
{
    if (lastFileFilter.isEmpty())
        lastFileFilter = FPBDQT_FILE_SUFFIX_PRO;

    QString path = QFileDialog::getExistingDirectory(this,
                                                     "Select target directory",
                                                     lastBuildPath);
    if (path.isEmpty())
        return;
    lastBuildPath = path;
    ui->textBuildPath->setText(path);

    ui->labelTargetFile->setText(path.append("/").
                                 append(getDefaultProjectName(
                                            ui->textProPath->text())));
}

void BuildWizard::on_BuildWizard_currentIdChanged(int id)
{
    bool accepted = true;

    switch(id)
    {
        case 1:
            if (!detectEssentials())
                accepted = false;
                break;
        case 2:
            if (ui->textProPath->text().isEmpty())
                accepted = false;
            break;
        case 3:
            if (ui->textBuildPath->text().isEmpty())
                accepted = false;

            if (ui->textRuntime->text().isEmpty())
            {
                QString rtName, rtVersion;
                if (detectRuntime(rtName, rtVersion))
                {
                    ui->textRuntime->setText(rtName);
                    ui->textRuntimeVer->setText(rtVersion);
                }
            }
            if (ui->textSDK->text().isEmpty())
            {
                QString sdkName;
                if (detectSdk(sdkName))
                {
                    ui->textSDK->setText(sdkName);
                }
            }
            break;
        case 4:
            if (ui->textAppID->text().isEmpty())
                accepted = false;
            if (ui->textRuntime->text().isEmpty())
                accepted = false;
            if (ui->textSDK->text().isEmpty())
                accepted = false;
            if (accepted)
            {
#ifndef QT_DEBUG
                button(WizardButton::BackButton)->setEnabled(false);
#endif
                buildStage = 0;
                startBuild();
            }
            break;
        case 5:
#ifndef QT_DEBUG
                button(WizardButton::BackButton)->setEnabled(false);
#endif
        default:;
    }

    if (!accepted)
        this->back();
}

void BuildWizard::onCuiStatusChanged(FlatpakLauncher::launcher_status status)
{
    if (status == FlatpakLauncher::finished)
        startBuild();
}
