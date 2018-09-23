#ifndef EXEPROBER_H
#define EXEPROBER_H

#include <QtGlobal>


class ExeProberPrivate;

class ExeProber
{
    Q_DECLARE_PRIVATE(ExeProber)
protected:
    ExeProberPrivate* d_ptr;

public:
    enum probe_error_code
    {
        probe_ok = 0,
        probe_cannot_start = 1,
        probe_process_stopped = 2,
        probe_process_crashed = 3,
        probe_file_not_exists = 128,
        probe_unknown_error = 255
    };

    ExeProber();
    probe_error_code probeExe(QString path);
    void setArguments(QList<QString> args);
    int returnValue();
    QByteArray output();
};

#endif // EXEPROBER_H
