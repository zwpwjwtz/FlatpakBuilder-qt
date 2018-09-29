#include "buildwizard.h"
#include "ui_buildwizard.h"
#include "dialogconfigmodule.h"
#include "aboutwindow.h"
#include "utils/exeprober.h"
#include "utils/flatpakhelper.h"
#include "utils/manifestcontainer.h"

#include <QFileDialog>
#include <QMessageBox>

#define FPBDQT_COMMAND_FLATPAK "/usr/bin/flatpak"
#define FPBDQT_COMMAND_FPBUILDER "/usr/bin/flatpak-builder"
#define FPBDQT_COMMAND_FPBUILDER_PROBEARGS "--version"
#define FPBDQT_COMMAND_SDK_QMAKE "qmake"
#define FPBDQT_COMMAND_SDK_QMAKE_PROBEARGS "--version"

#define FPBDQT_FILE_SUFFIX_ALL "All (*.*)(*.*)"
#define FPBDQT_FILE_SUFFIX_PRO "Qt Project File (*.pro)(*.pro)"
#define FPBDQT_FILE_LOG_DEFAULT "build_log.txt"

#define FPBDQT_APP_PREFIX_DEFAULT "com.example."
#define FPBDQT_APP_BUILDDIR_DEFAULT "build"
#define FPBDQT_APP_REPO_DEFAULT "repo"
#define FPBDQT_APP_BRANCH_DEFAULT "master"

#define FPBDQT_SOURCE_BRANCH_DEFAULT "master"


BuildWizard::BuildWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::BuildWizard)
{
    ui->setupUi(this);


    ui->tableModule->setHorizontalHeaderLabels({"ID",
                                                "Type",
                                                "Path / URL"});
    ui->tableModule->setColumnHidden(0, true);

    about = nullptr;
    moduleConfig = nullptr;

    connect(&builder,
           SIGNAL(builder_finished()),
           this,
           SLOT(onBuilderFinished()));
}

BuildWizard::~BuildWizard()
{
    delete ui;
    if (about)
        delete about;
    if (moduleConfig)
        delete moduleConfig;
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
    builder.setFlatpakBuilderPath(FPBDQT_COMMAND_FPBUILDER);
    builder.setFlatpakPath(FPBDQT_COMMAND_FLATPAK);

    /*
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
    */

    return true;

}


void BuildWizard::updateModuleList()
{
    ui->tableModule->setRowCount(0);
    QVector<int> sourceIDs = builder.sourceList(moduleID);
    for (int i=0; i<sourceIDs.count(); i++)
    {
        int sourceID = sourceIDs[i];
        ui->tableModule->insertRow(i);
        ui->tableModule->setItem(i, 0, new QTableWidgetItem(
                                                QString::number(sourceID)));
        ui->tableModule->setItem(i, 1, new QTableWidgetItem(
                                            builder.sourceTypeToString(
                                                builder.sourceType(sourceID))));
        ui->tableModule->setItem(i, 2, new QTableWidgetItem(
                                                builder.sourceURL(sourceID)));
    }
}

void BuildWizard::startBuild()
{
    builder.setAppID(ui->textAppID->text());
    builder.setBranch(FPBDQT_APP_BRANCH_DEFAULT);
    builder.setRuntime(ui->comboRuntime->currentText());
    builder.setSdkName(ui->comboSDK->currentText());
    builder.setRuntimeVer(ui->comboRuntimeVer->currentText());
    builder.setRunCmd(ui->textExeCmd->text());
    builder.setOwnName(ui->textOwnName->text());

    BuilderInstance::AppPermissions permission = BuilderInstance::AllowIPC |
                                                 BuilderInstance::Device_DRI |
                                                 BuilderInstance::FS_ReadWrite;
    if (ui->checkAudio->isChecked())
        permission |= BuilderInstance::AudioAccess;
    if (ui->checkNetwork->isChecked())
        permission |= BuilderInstance::NetworkAccess;
    if (ui->checkRootFS->isChecked())
        permission |= BuilderInstance::FS_RootDir;
    else
        permission |= BuilderInstance::FS_HomeDir;
    if (ui->checkSysBus->isChecked())
        permission |= BuilderInstance::SystemBusAccess;
    if (ui->checkX11->isChecked())
        permission |= BuilderInstance::X11Access;
    builder.setPermission(permission);

    int sourceID = builder.addSource(moduleID);
    QDir sourceDir(lastSrcPath);
    builder.setSourceType(sourceID, BuilderInstance::ScriptSource);
    builder.setSourceURL(sourceID, "configure");
    builder.setSourceCmd(sourceID, QString(FPBDQT_COMMAND_SDK_QMAKE)
                                        .append(" -config release ")
                                        .append(" PREFIX=/app ")
                                        .append(sourceDir.relativeFilePath(
                                                    ui->textProPath->text())));

    builder.setMakingExecutable(ui->checkBuildExecutable->isChecked());
    builder.setWorkingDir(ui->textBuildPath->text());
    builder.setBuildDir(ui->textBuildPath->text()
                        .append('/')
                        .append(FPBDQT_APP_BUILDDIR_DEFAULT));
    builder.setRepoDir(ui->textBuildPath->text()
                       .append('/')
                       .append(FPBDQT_APP_REPO_DEFAULT));
    builder.setBundleFile(ui->labelTargetFile->text());
    builder.setLogFile(ui->textBuildPath->text()
                        .append('/')
                        .append(FPBDQT_FILE_LOG_DEFAULT));
    builder.build();
}

void BuildWizard::onBuilderFinished()
{
    next();
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
    ui->textProPath->setText(path);

    QFileInfo proFile(path);
    lastSrcPath = proFile.dir().path();
    ui->labelSrcDir->setText(lastSrcPath);

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
            {
                accepted = false;
                break;
            }

            if (ui->comboRuntime->currentText().isEmpty())
            {
                QList<FlatpakHelper::RuntimeInfo> runtimeList =
                                                FlatpakHelper::getRuntimeList();
                for (int i=0; i<runtimeList.count(); i++)
                {
                    ui->comboRuntime->addItem(runtimeList[i].name);
                    if (ui->comboRuntimeVer->findText(runtimeList[i].version) < 0)
                        ui->comboRuntimeVer->addItem(runtimeList[i].version);
                }
            }
            if (ui->comboSDK->currentText().isEmpty())
            {
                QList<FlatpakHelper::RuntimeInfo> sdkList =
                                                    FlatpakHelper::getSDKList();
                for (int i=0; i<sdkList.count(); i++)
                    ui->comboSDK->addItem(sdkList[i].name);
            }
            if (builder.moduleList().isEmpty())
            {
                moduleID = builder.addModule();
                builder.setModuleName(moduleID, appName);
            }
            if (ui->tableModule->rowCount() < 1)
            {
                int sourceID = builder.addSource(moduleID);
                builder.setSourceType(sourceID, BuilderInstance::GitSource);
                builder.setSourceURL(sourceID,
                                     QString("file://").append(lastSrcPath));
                builder.setSourceVer(sourceID, FPBDQT_SOURCE_BRANCH_DEFAULT);
                updateModuleList();
            }
            if (ui->textExeCmd->text().isEmpty())
                ui->textExeCmd->setText(appName);
            if (ui->textOwnName->text().isEmpty())
                ui->textOwnName->setText(ui->textAppID->text());
            break;
        case 4:
            if (ui->textAppID->text().isEmpty())
                accepted = false;
            if (ui->comboRuntime->currentText().isEmpty() ||
                ui->comboSDK->currentText().isEmpty() ||
                ui->comboSDK->currentText().isEmpty())
                accepted = false;
            if (accepted)
            {
#ifndef QT_DEBUG
                button(WizardButton::BackButton)->setEnabled(false);
#endif
                startBuild();
            }
            break;
        case 5:
#ifndef QT_DEBUG
                button(WizardButton::BackButton)->setEnabled(false);
#endif
            ui->labelBuildResult->setText(
                        QString("Building finished. You can find built package "
                                "as well as build log <a href=\"%1\">here</a>.")
                               .arg(builder.workingDir().prepend("file://")));
        default:;
    }

    if (!accepted)
        this->back();
}

void BuildWizard::on_buttonModuleAdd_clicked()
{
    builder.addSource(moduleID);
    updateModuleList();
}

void BuildWizard::on_buttonModuleRemove_clicked()
{
    int selectedIndex = ui->tableModule->currentRow();
    if (selectedIndex < 0)
        return;
    int sourceID = ui->tableModule->item(selectedIndex, 0)
                    ->data(0).toInt();
    builder.removeSource(sourceID);
    updateModuleList();
}

void BuildWizard::on_buttonModuleConfig_clicked()
{
    int selectedIndex = ui->tableModule->currentRow();
    if (selectedIndex < 0)
        return;
    int sourceID = ui->tableModule->item(selectedIndex, 0)
                    ->data(0).toInt();

    if (moduleConfig == nullptr)
        moduleConfig = new DialogConfigModule;
    moduleConfig->sourceType = builder.sourceType(sourceID);
    moduleConfig->sourceURL = builder.sourceURL(sourceID);
    moduleConfig->sourceVersion = builder.sourceVer(sourceID);
    moduleConfig->sourceCommand = builder.sourceCmd(sourceID);
    moduleConfig->exec();
    builder.setSourceType(sourceID, moduleConfig->sourceType);
    builder.setSourceURL(sourceID, moduleConfig->sourceURL);
    builder.setSourceVer(sourceID, moduleConfig->sourceVersion);
    builder.setSourceCmd(sourceID, moduleConfig->sourceCommand);
    updateModuleList();
}

void BuildWizard::on_tableModule_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    on_buttonModuleConfig_clicked();
}

void BuildWizard::on_labelChangeSrcDir_linkActivated(const QString &link)
{
    Q_UNUSED(link)

    QString path = QFileDialog::getExistingDirectory(this,
                                                     "Select source directory",
                                                     lastSrcPath);
    if (path.isEmpty())
        return;
    if (!ui->textProPath->text().contains(path))
    {
        QMessageBox::critical(this, "Path error",
                              "The project file must be within "
                              "the source directory.");
        return;
    }

    lastSrcPath = path;
    ui->labelSrcDir->setText(lastSrcPath);
}

void BuildWizard::on_buttonAbout_clicked()
{
    if (!about)
        about = new AboutWindow(this);
    about->show();
}

void BuildWizard::on_labelChangeTargetPath_linkActivated(const QString &link)
{
    Q_UNUSED(link)

    QString path = QFileDialog::getSaveFileName(this,
                                                "Select target file",
                                                ui->labelTargetFile->text());
    if (!path.isEmpty())
        ui->labelTargetFile->setText(path);
}
