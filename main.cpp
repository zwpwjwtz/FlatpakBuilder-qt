#include "buildwizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Enable scaling for HiDPI device
    if (qgetenv("QT_SCALE_FACTOR").isEmpty() &&
        qgetenv("QT_SCREEN_SCALE_FACTORS").isEmpty())
    {
        qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    }

    QApplication a(argc, argv);
    BuildWizard w;
    w.show();

    return a.exec();
}
