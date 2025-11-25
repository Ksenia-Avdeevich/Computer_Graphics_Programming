#include "imageinfo.h"
#include <QElapsedTimer>
#include <QFileInfo>
#include <QImageReader>
#include <QFile>
#include <cmath>

QString getCompressionInfo(const QString &format)
{
    QString f = format.toUpper();
    if (f == "JPG" || f == "JPEG") return "JPEG";
    if (f == "PNG") return "Deflate";
    if (f == "GIF") return "LZW";
    if (f == "BMP") return "Без сжатия";
    if (f == "TIF" || f == "TIFF") return "LZW/Deflate/JPEG";
    if (f == "PCX") return "RLE";
    return "Неизвестно";
}

QString getAdditionalInfo(const QString &format, const QImage &image)
{
    QStringList details;
    QString f = format.toUpper();

    // Цветовое пространство
    if (f == "JPG" || f == "JPEG") {
        details << "YCbCr";
    } else if (f == "PNG" || f == "BMP") {
        details << "RGB";
    } else if (f == "GIF") {
        details << "Indexed";
    } else {
        details << "неизвестно";
    }

    // Тип изображения
    if (image.isGrayscale()) {
        details << "Grayscale";
    } else if (image.colorCount() > 0) {
        details << "Indexed";
    } else {
        details << "Truecolor";
    }

    // Каналы
    if (!image.isNull()) {
        if (image.isGrayscale()) {
            details << "1 канал";
        } else if (image.hasAlphaChannel()) {
            details << "4 канала (RGBA)";
            details << "Прозрачность есть";
        } else {
            details << "3 канала (RGB)";
            details << "Нет прозрачности";
        }
    } else {
        details << "Каналов: неизвестно";
    }

    // Соотношение сторон
    int w = image.width();
    int h = image.height();
    if (w > 0 && h > 0) {
        int gcd = std::gcd(w, h);
        details << QString("Соотношение: %1:%2").arg(w / gcd).arg(h / gcd);
    }

    return details.join(", ");
}

// Функция для расчёта степени сжатия
QString calculateCompressionRatio(const QString &filePath, const QImage &image, const QString &format)
{
    if (image.isNull()) return "N/A";

    QFileInfo fi(filePath);
    qint64 actualSize = fi.size();

    // Расчёт несжатого размера
    int width = image.width();
    int height = image.height();
    int bytesPerPixel = image.depth() / 8;
    qint64 uncompressedSize = static_cast<qint64>(width) * height * bytesPerPixel;

    QString f = format.toUpper();
    if (f == "BMP") {
        // BMP может иметь заголовок
        return "-";
    }

    if (actualSize > 0 && uncompressedSize > 0) {
        double savedMB = (uncompressedSize - actualSize) / (1024.0 * 1024.0);
        double savedPercent = ((uncompressedSize - actualSize) * 100.0) / uncompressedSize;

        return QString("-%1 МБ, -%2%")
            .arg(savedMB, 0, 'f', 2)
            .arg(savedPercent, 0, 'f', 1);
    }

    return "N/A";
}

// Функция для извлечения матрицы квантования из JPEG
QVector<QVector<int>> extractQuantizationMatrix(const QString &filePath)
{
    QVector<QVector<int>> matrix;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        return matrix;
    }

    QByteArray data = file.readAll();
    file.close();

    // Поиск маркера DQT (Define Quantization Table) - 0xFFDB
    for (int i = 0; i < data.size() - 1; i++) {
        if (static_cast<unsigned char>(data[i]) == 0xFF &&
            static_cast<unsigned char>(data[i + 1]) == 0xDB) {

            // Найден маркер DQT
            if (i + 4 >= data.size()) break;

            int length = (static_cast<unsigned char>(data[i + 2]) << 8) |
                         static_cast<unsigned char>(data[i + 3]);

            if (i + 4 + length - 2 > data.size()) break;

            int offset = i + 4;

            // Читаем информацию о таблице
            unsigned char tableInfo = static_cast<unsigned char>(data[offset]);
            int precision = (tableInfo >> 4) & 0x0F;  // 0 = 8 бит, 1 = 16 бит
            int tableId = tableInfo & 0x0F;

            offset++;

            // Читаем 8x8 матрицу
            if (precision == 0 && offset + 64 <= data.size()) {
                matrix.resize(8);
                for (int row = 0; row < 8; row++) {
                    matrix[row].resize(8);
                    for (int col = 0; col < 8; col++) {
                        // JPEG использует зигзаг-порядок, но для простоты читаем последовательно
                        matrix[row][col] = static_cast<unsigned char>(data[offset++]);
                    }
                }
                break;  // Берём первую найденную таблицу
            }
        }
    }

    return matrix;
}

ImageInfo getImageInfo(const QString &filePath)
{
    ImageInfo info;
    QFileInfo fi(filePath);
    QImageReader reader(filePath);
    QImage image(filePath);

    info.fileName = fi.fileName();
    info.fileSize = QString("%1 KB").arg(fi.size() / 1024.0, 0, 'f', 1);
    info.format = reader.format().toUpper();

    QSize size = reader.size();
    if (size.isValid()) {
        info.size = QString("%1 x %2").arg(size.width()).arg(size.height());
    } else {
        info.size = image.isNull() ? "Некорректный размер" : QString("%1 x %2").arg(image.width()).arg(image.height());
    }

    int dpiX = static_cast<int>(image.dotsPerMeterX() * 0.0254 + 0.5);
    int dpiY = static_cast<int>(image.dotsPerMeterY() * 0.0254 + 0.5);

    info.resolution = QString("%1 x %2").arg(dpiX).arg(dpiY);
    info.colorDepth = image.isNull() ? "Неизвестно" : QString("%1 бит").arg(image.depth());
    info.compression = getCompressionInfo(info.format);
    info.additionalInfo = getAdditionalInfo(info.format, image);

    // Расчёт степени сжатия
    info.compressionRatio = calculateCompressionRatio(filePath, image, info.format);

    // Извлечение матрицы квантования для JPEG
    info.hasQuantMatrix = false;
    QString f = info.format.toUpper();
    if (f == "JPG" || f == "JPEG") {
        info.quantizationMatrix = extractQuantizationMatrix(filePath);
        info.hasQuantMatrix = !info.quantizationMatrix.isEmpty();
    }

    return info;
}
