#include <QFile>
#include <QCryptographicHash>
#include "checksumcalculator.h"

#define CHECKSUMCALC_FILE_READ_BLOCKLEN 1024*1024


ChecksumCalculator::ChecksumCalculator()
{
}

QByteArray ChecksumCalculator::getFileChecksum(QString fileName, ChecksumType type)
{
    QFile inputFile(fileName);
    if (!inputFile.exists())
        return "";

    QCryptographicHash::Algorithm algorithm;
    switch (type)
    {
        case md5:
            algorithm = QCryptographicHash::Md5;
            break;
        case sha1:
            algorithm = QCryptographicHash::Sha1;
            break;
        default:
            return "";
    }

    QCryptographicHash hasher(algorithm);
    inputFile.open(QFile::ReadOnly);
    while (!inputFile.atEnd())
    {
        hasher.addData(inputFile.read(CHECKSUMCALC_FILE_READ_BLOCKLEN));
    }

    return hasher.result();
}

QString ChecksumCalculator::getFileChecksumString(QString fileName, ChecksumType type)
{
    return getFileChecksum(fileName, type).toHex();
}
