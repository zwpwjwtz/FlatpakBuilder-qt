#include <QShowEvent>
#include "dialogconfigmodule.h"
#include "ui_dialogconfigmodule.h"

DialogConfigModule::DialogConfigModule(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConfigModule)
{
    ui->setupUi(this);
}

DialogConfigModule::~DialogConfigModule()
{
    delete ui;
}

void DialogConfigModule::showEvent(QShowEvent* event)
{
    int typeIndex;
    switch(sourceType)
    {
        case BuilderInstance::ArchiveSource:
            typeIndex = 3;
            break;
        case BuilderInstance::GitSource:
            typeIndex = 0;
            break;
        case BuilderInstance::BzrSource:
            typeIndex = 1;
            break;
        case BuilderInstance::FileSource:
            typeIndex = 4;
            break;
        case BuilderInstance::ScriptSource:
            typeIndex = 5;
            break;
        case BuilderInstance::ShellSource:
            typeIndex = 6;
            break;
        case BuilderInstance::PatchSource:
            typeIndex = 2;
            break;
        default:
            typeIndex = -1;
    }
    ui->comboBox->setCurrentIndex(typeIndex);
    ui->textURL->setText(sourceURL);
    ui->textVersion->setText(sourceVersion);
    ui->textCommand->setText(sourceCommand);
    event->accept();
}

void DialogConfigModule::on_buttonBox_accepted()
{
    switch (ui->comboBox->currentIndex()) {
        case 0:
            sourceType = BuilderInstance::GitSource;
            break;
        case 1:
            sourceType = BuilderInstance::BzrSource;
            break;
        case 2:
            sourceType = BuilderInstance::PatchSource;
            break;
        case 3:
            sourceType = BuilderInstance::ArchiveSource;
            break;
        case 4:
            sourceType = BuilderInstance::FileSource;
            break;
        case 5:
            sourceType = BuilderInstance::ScriptSource;
            break;
        case 6:
            sourceType = BuilderInstance::ShellSource;
            break;
        default:
            sourceType = BuilderInstance::UnknownSource;
    }
    sourceURL = ui->textURL->text();
    sourceVersion = ui->textVersion->text();
    sourceCommand = ui->textCommand->text();
}
