#ifndef BUILDWIZARD_H
#define BUILDWIZARD_H

#include <QWizard>
#include "utils/builderinstance.h"


class DialogConfigModule;
class AboutWindow;

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
    void onBuilderFinished();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_BuildWizard_currentIdChanged(int id);
    void on_buttonModuleAdd_clicked();
    void on_buttonModuleRemove_clicked();
    void on_buttonModuleConfig_clicked();
    void on_tableModule_doubleClicked(const QModelIndex &index);
    void on_labelChangeSrcDir_linkActivated(const QString &link);
    void on_buttonAbout_clicked();

private:
    Ui::BuildWizard *ui;
    AboutWindow* about;
    DialogConfigModule *moduleConfig;
    QString appName;
    QString lastSrcPath;
    QString lastBuildPath;
    QString lastFileFilter;
    BuilderInstance builder;
    int moduleID;

    QString getDefaultProjectName(QString fileName);
    bool detectEssentials();
    bool detectRuntime(QString& rtName, QString& rtVersion);
    bool detectSdk(QString& sdkName);
    QString findFirstDirEntry(QString path);
    void updateModuleList();
    void startBuild();
};

#endif // BUILDWIZARD_H
