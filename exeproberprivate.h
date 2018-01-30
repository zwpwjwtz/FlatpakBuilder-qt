#ifndef EXEPROBERPRIVATE_H
#define EXEPROBERPRIVATE_H

#include <QProcess>


class ExeProberPrivate : QObject
{
    Q_DECLARE_PUBLIC(ExeProber)

public:

    QProcess process;
    QList<QString> arguments;

    ExeProberPrivate();
    bool fileExists(QString path);

protected:
    ExeProber* q_ptr;
};

#endif // EXEPROBERPRIVATE_H
