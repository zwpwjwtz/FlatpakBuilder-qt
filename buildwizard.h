#ifndef BUILDWIZARD_H
#define BUILDWIZARD_H

#include <QWizard>

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


private:
    Ui::BuildWizard *ui;
    QString lastSrcPath;
    QString lastBuildPath;
    QString lastFileFilter;

    QString getDefaultProjectName(QString fileName);
};

#endif // BUILDWIZARD_H
