#ifndef IMAGEOPERATIONS_H
#define IMAGEOPERATIONS_H

#include <QImage>
#include <QVector>

class ImageOperations {
public:
    static QImage adjustContrast(const QImage& img);
    static QImage balanceRGB(const QImage& img);
    static QImage balanceHSV(const QImage& img);
    static QImage enhanceSharpness(const QImage& img);

private:
    static QVector<int> calculateHistogram(const QImage& img, int channel);
    static QVector<uchar> createEqualizationTable(const QVector<int>& hist, int totalPixels);
};

#endif // IMAGEOPERATIONS_H
