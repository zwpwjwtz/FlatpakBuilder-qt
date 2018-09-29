#ifndef CHECKSUMCALCULATOR_H
#define CHECKSUMCALCULATOR_H

#include <QtGlobal>


class ChecksumCalculator
{
public:
    enum ChecksumType
    {
        md5 = 1,
        sha1 = 2,
    };

    ChecksumCalculator();
    static QByteArray getFileChecksum(QString fileName, ChecksumType type);
    static QString getFileChecksumString(QString fileName, ChecksumType type);
};

#endif // CHECKSUMCALCULATOR_H
