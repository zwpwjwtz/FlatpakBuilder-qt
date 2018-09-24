#ifndef DIALOGCONFIGMODULE_H
#define DIALOGCONFIGMODULE_H

#include <QDialog>
#include "utils/builderinstance.h"


namespace Ui {
class DialogConfigModule;
}

class DialogConfigModule : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfigModule(QWidget *parent = 0);
    ~DialogConfigModule();

    BuilderInstance::SourceType sourceType;
    QString sourceURL;
    QString sourceVersion;
    QString sourceCommand;

    void showEvent(QShowEvent* event);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogConfigModule *ui;
};

#endif // DIALOGCONFIGMODULE_H
