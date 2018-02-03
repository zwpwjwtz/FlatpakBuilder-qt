#ifndef BUILDWIZARD_H
#define BUILDWIZARD_H

#include <QWizard>
#include "flatpaklauncher.h"


namespace Ui {
class BuildWizard;
}

class BuildWizard : public QWizard
{
    Q_OBJECT

public:
    explicit BuildWizard(QWidget *parent = 0);
    ~BuildWizard();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_BuildWizard_currentIdChanged(int id);
    void onCuiStatusChanged(FlatpakLauncher::launcher_status status);

private:
    Ui::BuildWizard *ui;
    QString appName;
    QString lastSrcPath;
    QString lastBuildPath;
    QString lastFileFilter;
    FlatpakLauncher fp_cui;
    int buildStage;

    QString getDefaultProjectName(QString fileName);
    bool detectEssentials();
    bool detectRuntime(QString& rtName, QString& rtVersion);
    bool detectSdk(QString& sdkName);
    QString findFirstDirEntry(QString path);
    bool logCuiOutput(QString filePath, bool append);
    void startBuild();
};

#endif // BUILDWIZARD_H
