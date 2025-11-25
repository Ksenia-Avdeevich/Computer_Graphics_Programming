#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QString>
#include <QImage>
#include <QVector>
#include <QMetaType>

struct ImageInfo {
    QString fileName;
    QString size;
    QString resolution;
    QString colorDepth;
    QString compression;
    QString format;
    QString fileSize;
    QString additionalInfo;
    QString compressionRatio;
    QVector<QVector<int>> quantizationMatrix;  // Матрица квантования для JPEG
    bool hasQuantMatrix;  // Флаг наличия матрицы квантования
};

Q_DECLARE_METATYPE(QVector<ImageInfo>)

ImageInfo getImageInfo(const QString &filePath);

#endif // IMAGEINFO_H
