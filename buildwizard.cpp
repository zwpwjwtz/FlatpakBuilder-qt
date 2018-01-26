#include "buildwizard.h"
#include "ui_buildwizard.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

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
