#include "buildwizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BuildWizard w;
    w.show();

    return a.exec();
}
