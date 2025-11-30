#include "rasteralgorithms.h"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <QSet>
#include <QPair>

using namespace std::chrono;

namespace {
AlgorithmStep createStep(int x, int y, const QString &description = QString(), double intensity = 1.0) {
    AlgorithmStep step;
    step.setCoordinates(Point(x, y));
    step.setDescription(description);
    step.setIntensity(intensity);
    return step;
}

void swapValues(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

void swapValues(double &a, double &b) {
    double temp = a;
    a = b;
    b = temp;
}

void addUniqueStep(QVector<AlgorithmStep> &steps, const AlgorithmStep &newStep) {
    if (steps.isEmpty()) {
        steps.append(newStep);
        return;
    }

    Point lastPoint = steps.last().getCoordinates();
    Point newPoint = newStep.getCoordinates();

    if (lastPoint.x() != newPoint.x() || lastPoint.y() != newPoint.y()) {
        steps.append(newStep);
    }
}

int calculateDistance(int x1, int y1, int x2, int y2) {
    return std::max(std::abs(x2 - x1), std::abs(y2 - y1));
}

double fractionalPart(double x) {
    return x - std::floor(x);
}

double reverseFractionalPart(double x) {
    return 1.0 - fractionalPart(x);
}
}

AlgorithmResult RasterAlgorithms::stepByStep(const Point &start, const Point &end) {
    AlgorithmResult result;
    auto startTime = high_resolution_clock::now();

    int x0 = start.x(), y0 = start.y();
    int x1 = end.x(), y1 = end.y();
    int dx = x1 - x0, dy = y1 - y0;

    QVector<AlgorithmStep> steps;

    if (dx == 0 && dy == 0) {
        // Single point
        steps.append(createStep(x0, y0, "Single point", 1.0));
    } else if (dx == 0) {
        // Vertical line
        int stepY = (dy > 0) ? 1 : -1;
        for (int y = y0; y != y1 + stepY; y += stepY) {
            addUniqueStep(steps, createStep(x0, y, QString("y=%1").arg(y), 1.0));
        }
    } else {
        // General case
        double slope = double(dy) / double(dx);
        int stepX = (dx > 0) ? 1 : -1;

        for (int x = x0; x != x1 + stepX; x += stepX) {
            double exactY = y0 + slope * (x - x0);
            int roundedY = int(std::round(exactY));
            addUniqueStep(steps, createStep(x, roundedY, QString("y=%.2f").arg(exactY), 1.0));
        }
    }

    result.setSteps(steps);
    result.setExecutionTime(high_resolution_clock::now() - startTime);
    return result;
}

AlgorithmResult RasterAlgorithms::digitalDifferentialAnalyzer(const Point &start, const Point &end) {
    AlgorithmResult result;
    auto startTime = high_resolution_clock::now();

    int x0 = start.x(), y0 = start.y();
    int x1 = end.x(), y1 = end.y();

    QVector<AlgorithmStep> steps;

    int length = calculateDistance(x0, y0, x1, y1);

    if (length == 0) {
        steps.append(createStep(x0, y0, "Single point", 1.0));
        result.setSteps(steps);
        result.setExecutionTime(high_resolution_clock::now() - startTime);
        return result;
    }

    double xIncrement = double(x1 - x0) / length;
    double yIncrement = double(y1 - y0) / length;

    double currentX = x0;
    double currentY = y0;

    for (int i = 0; i <= length; ++i) {
        int pixelX = int(std::round(currentX));
        int pixelY = int(std::round(currentY));
        addUniqueStep(steps, createStep(pixelX, pixelY, QString("Step %1").arg(i), 1.0));

        currentX += xIncrement;
        currentY += yIncrement;
    }

    result.setSteps(steps);
    result.setExecutionTime(high_resolution_clock::now() - startTime);
    return result;
}

AlgorithmResult RasterAlgorithms::bresenhamLine(const Point &start, const Point &end) {
    AlgorithmResult result;
    auto startTime = high_resolution_clock::now();

    int x0 = start.x(), y0 = start.y();
    int x1 = end.x(), y1 = end.y();

    QVector<AlgorithmStep> steps;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int stepX = (x0 < x1) ? 1 : -1;
    int stepY = (y0 < y1) ? 1 : -1;

    int error = dx - dy;
    int currentX = x0;
    int currentY = y0;

    while (true) {
        addUniqueStep(steps, createStep(currentX, currentY, QString("Error=%1").arg(error), 1.0));

        if (currentX == x1 && currentY == y1) break;

        int doubleError = 2 * error;

        if (doubleError > -dy) {
            error -= dy;
            currentX += stepX;
        }

        if (doubleError < dx) {
            error += dx;
            currentY += stepY;
        }
    }

    result.setSteps(steps);
    result.setExecutionTime(high_resolution_clock::now() - startTime);
    return result;
}

AlgorithmResult RasterAlgorithms::bresenhamCircle(const Point &center, const Point &radiusPoint) {
    AlgorithmResult result;
    auto startTime = high_resolution_clock::now();

    int centerX = center.x(), centerY = center.y();
    int radius = std::abs(radiusPoint.x());

    QVector<AlgorithmStep> steps;

    if (radius <= 0) {
        steps.append(createStep(centerX, centerY, "Zero radius", 1.0));
        result.setSteps(steps);
        result.setExecutionTime(high_resolution_clock::now() - startTime);
        return result;
    }

    QSet<QPair<int, int>> drawnPixels;

    auto addPixel = [&](int x, int y) {
        QPair<int, int> pixel(x, y);
        if (!drawnPixels.contains(pixel)) {
            drawnPixels.insert(pixel);
            addUniqueStep(steps, createStep(x, y, QString(), 1.0));
        }
    };

    int x = 0;
    int y = radius;
    int decisionParameter = 3 - 2 * radius;

    while (x <= y) {
        // Draw all eight symmetric points
        addPixel(centerX + x, centerY + y);
        addPixel(centerX - x, centerY + y);
        addPixel(centerX + x, centerY - y);
        addPixel(centerX - x, centerY - y);
        addPixel(centerX + y, centerY + x);
        addPixel(centerX - y, centerY + x);
        addPixel(centerX + y, centerY - x);
        addPixel(centerX - y, centerY - x);

        if (decisionParameter < 0) {
            decisionParameter = decisionParameter + 4 * x + 6;
        } else {
            decisionParameter = decisionParameter + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }

    result.setSteps(steps);
    result.setExecutionTime(high_resolution_clock::now() - startTime);
    return result;
}


AlgorithmResult RasterAlgorithms::wuAntiAliasedLine(const Point &start, const Point &end) {
    AlgorithmResult result;
    auto startTime = high_resolution_clock::now();

    double x0 = start.x(), y0 = start.y();
    double x1 = end.x(), y1 = end.y();

    QVector<AlgorithmStep> steps;

    // Проверка вырожденного случая
    if (std::abs(x1 - x0) < 1e-6 && std::abs(y1 - y0) < 1e-6) {
        steps.append(createStep(int(x0), int(y0), "Single point", 1.0));
        result.setSteps(steps);
        result.setExecutionTime(high_resolution_clock::now() - startTime);
        return result;
    }

    // Определяем, является ли линия крутой (наклон > 45 градусов)
    bool isSteep = std::abs(y1 - y0) > std::abs(x1 - x0);

    // Если линия крутая, меняем координаты x и y местами
    if (isSteep) {
        swapValues(x0, y0);
        swapValues(x1, y1);
    }

    // Гарантируем, что рисуем слева направо
    if (x0 > x1) {
        swapValues(x0, x1);
        swapValues(y0, y1);
    }

    double dx = x1 - x0;
    double dy = y1 - y0;
    double gradient = (std::abs(dx) < 1e-6) ? 1.0 : dy / dx;

    // Функция для добавления пикселя с учетом интенсивности
    auto plotPixel = [&](double x, double y, double intensity, const QString &description = "") {
        int pixelX, pixelY;
        if (isSteep) {
            pixelX = int(std::round(y));
            pixelY = int(std::round(x));
        } else {
            pixelX = int(std::round(x));
            pixelY = int(std::round(y));
        }
        addUniqueStep(steps, createStep(pixelX, pixelY, description, intensity));
    };

    // Обрабатываем первую точку
    double xEnd = std::round(x0);
    double yEnd = y0 + gradient * (xEnd - x0);
    double xGap = reverseFractionalPart(x0 + 0.5);

    int xPixel1 = int(xEnd);
    int yPixel1 = int(std::floor(yEnd));

    // Рисуем два пикселя для первой точки с разной интенсивностью
    plotPixel(xPixel1, yPixel1, reverseFractionalPart(yEnd) * xGap,
              QString("First point - main: %1").arg(reverseFractionalPart(yEnd) * xGap, 0, 'f', 2));
    plotPixel(xPixel1, yPixel1 + 1, fractionalPart(yEnd) * xGap,
              QString("First point - secondary: %1").arg(fractionalPart(yEnd) * xGap, 0, 'f', 2));

    double intery = yEnd + gradient; // Y-пересечение для следующего шага

    // Обрабатываем вторую точку
    xEnd = std::round(x1);
    yEnd = y1 + gradient * (xEnd - x1);
    xGap = fractionalPart(x1 + 0.5);

    int xPixel2 = int(xEnd);
    int yPixel2 = int(std::floor(yEnd));

    // Рисуем два пикселя для последней точки с разной интенсивностью
    plotPixel(xPixel2, yPixel2, reverseFractionalPart(yEnd) * xGap,
              QString("Last point - main: %1").arg(reverseFractionalPart(yEnd) * xGap, 0, 'f', 2));
    plotPixel(xPixel2, yPixel2 + 1, fractionalPart(yEnd) * xGap,
              QString("Last point - secondary: %1").arg(fractionalPart(yEnd) * xGap, 0, 'f', 2));

    // Основной цикл - рисуем промежуточные точки
    for (int x = xPixel1 + 1; x <= xPixel2 - 1; ++x) {
        // Рисуем два пикселя для каждой x-координаты с разной интенсивностью
        plotPixel(x, int(std::floor(intery)), reverseFractionalPart(intery),
                  QString("X=%1, lower: %2").arg(x).arg(reverseFractionalPart(intery), 0, 'f', 2));
        plotPixel(x, int(std::floor(intery)) + 1, fractionalPart(intery),
                  QString("X=%1, upper: %2").arg(x).arg(fractionalPart(intery), 0, 'f', 2));

        intery += gradient;
    }

    // Убираем дубликаты и объединяем интенсивности для одинаковых пикселей
    QVector<AlgorithmStep> optimizedSteps;
    QMap<QPair<int, int>, double> pixelIntensities;

    for (const AlgorithmStep &step : steps) {
        Point p = step.getCoordinates();
        QPair<int, int> key(p.x(), p.y());

        if (pixelIntensities.contains(key)) {
            // Берем максимальную интенсивность для пикселя
            pixelIntensities[key] = std::max(pixelIntensities[key], step.getIntensity());
        } else {
            pixelIntensities[key] = step.getIntensity();
        }
    }

    // Создаем финальный список шагов
    for (auto it = pixelIntensities.begin(); it != pixelIntensities.end(); ++it) {
        int x = it.key().first;
        int y = it.key().second;
        double intensity = it.value();

        optimizedSteps.append(createStep(x, y,
                                         QString("Final intensity: %1").arg(intensity, 0, 'f', 2),
                                         intensity));
    }

    result.setSteps(optimizedSteps);
    result.setExecutionTime(high_resolution_clock::now() - startTime);
    return result;
}
