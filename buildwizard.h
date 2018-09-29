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
    void onBuilderStaged(BuilderInstance::BuilderStage stage);
    void onBuilderError(BuilderInstance::BuilderErrorCode errCode);
    void onBuilderOutputUpdated();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_BuildWizard_currentIdChanged(int id);
    void on_buttonModuleAdd_clicked();
    void on_buttonModuleRemove_clicked();
    void on_buttonModuleConfig_clicked();
    void on_tableModule_doubleClicked(const QModelIndex &index);
    void on_labelChangeSrcDir_linkActivated(const QString &link);
    void on_buttonAbout_clicked();
    void on_labelChangeTargetPath_linkActivated(const QString &link);
    void on_buttonShowProgressDetails_clicked();
    void on_buttonHideProgressDetails_clicked();

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
    int buildLogSize;

    bool detectEssentials();
    QString getDefaultProjectName(QString fileName);
    void updateModuleList();
    void startBuild();
    void updateBuildDetails();
};

#endif // BUILDWIZARD_H
