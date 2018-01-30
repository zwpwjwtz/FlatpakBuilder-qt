#include "buildwizard.h"
#include "ui_buildwizard.h"
#include "exeprober.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#define FPBDQT_COMMAND_FPBUILDER "/usr/bin/flatpak-builder"
#define FPBDQT_COMMAND_FPBUILDER_PROBEARGS "--version"
#define FPBDQT_COMMAND_QMAKE "/usr/bin/qmake"
#define FPBDQT_COMMAND_QMAKE_PROBEARGS "--version"

#define FPBDQT_FILE_SUFFIX_ALL "All (*.*)(*.*)"
#define FPBDQT_FILE_SUFFIX_PRO "Qt Project File (*.pro)(*.pro)"


BuildWizard::BuildWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::BuildWizard)
{
    ui->setupUi(this);
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
            break;
    }

    if (!accepted)
        this->back();
}
