#include "rasteralgorithms.h"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <QSet>
#include <QPair>
#include <QMap>
#include <QPoint>

using namespace std::chrono;

namespace {
AlgorithmStep createStep(int x, int y, const QString &description = QString(), double intensity = 1.0) {
    AlgorithmStep step;
    step.setCoordinates(Point(x, y));
    step.setDescription(description);
    step.setIntensity(intensity);
    return step;
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

    // 1. Одна точка
    if (dx == 0 && dy == 0) {
        steps.append(createStep(x0, y0, "Single point", 1.0));
    }
    // 2. Вертикальная линия
    else if (dx == 0) {
        int stepY = (dy > 0) ? 1 : -1;
        for (int y = y0; ; y += stepY) {
            addUniqueStep(steps, createStep(x0, y, QString("x=%1, y=%2").arg(x0).arg(y), 1.0));
            if (y == y1) break;
        }
    }
    // 3. Горизонтальная линия
    else if (dy == 0) {
        int stepX = (dx > 0) ? 1 : -1;
        for (int x = x0; ; x += stepX) {
            addUniqueStep(steps, createStep(x, y0, QString("x=%1, y=%2").arg(x).arg(y0), 1.0));
            if (x == x1) break;
        }
    }
    // 4. Общий случай (линия с наклоном)
    else {
        double slope = double(dy) / double(dx);

        // Определяем направление движения по X
        int stepX = (dx > 0) ? 1 : -1;

        // Идем от x0 до x1 включительно
        int stepsCount = std::abs(dx) + 1;

        for (int i = 0; i < stepsCount; ++i) {
            int currentX = x0 + i * stepX;
            double exactY = y0 + slope * (currentX - x0);

            // Правильное округление для отрицательных чисел
            int roundedY;
            if (exactY >= 0) {
                roundedY = int(exactY + 0.5);
            } else {
                roundedY = int(exactY - 0.5);
            }

            addUniqueStep(steps, createStep(
                                     currentX,
                                     roundedY,
                                     QString("x=%1, exact y=%.2f, rounded y=%2")
                                         .arg(currentX)
                                         .arg(exactY)
                                         .arg(roundedY),
                                     1.0
                                     ));
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
    int dx = x1 - x0, dy = y1 - y0;

    QVector<AlgorithmStep> steps;
    int L = std::max(std::abs(dx), std::abs(dy));

    if (L == 0) {
        steps.append(createStep(x0, y0, "Single point", 1.0));
        result.setSteps(steps);
        result.setExecutionTime(high_resolution_clock::now() - startTime);
        return result;
    }

    double xIncrement = double(dx) / L;
    double yIncrement = double(dy) / L;

    double currentX = x0 + 0.5; // Start from pixel center
    double currentY = y0 + 0.5;

    for (int i = 0; i <= L; ++i) {
        int pixelX = int(std::floor(currentX));
        int pixelY = int(std::floor(currentY));
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

    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

    // If line is steep, swap x and y
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    // Ensure drawing from left to right
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int error = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;

    for (int x = x0; x <= x1; ++x) {
        if (steep) {
            addUniqueStep(steps, createStep(y, x, QString("Error=%1").arg(error), 1.0));
        } else {
            addUniqueStep(steps, createStep(x, y, QString("Error=%1").arg(error), 1.0));
        }

        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }

    result.setSteps(steps);
    result.setExecutionTime(high_resolution_clock::now() - startTime);
    return result;
}

AlgorithmResult RasterAlgorithms::bresenhamCircle(const Point &center, const Point &radiusPoint) {
    AlgorithmResult result;
    auto startTime = high_resolution_clock::now();

    // Calculate radius as distance between center and radiusPoint
    int dx = radiusPoint.x() - center.x();
    int dy = radiusPoint.y() - center.y();
    int radius = int(std::sqrt(dx*dx + dy*dy));

    QVector<AlgorithmStep> steps;

    if (radius <= 0) {
        steps.append(createStep(center.x(), center.y(), "Zero radius", 1.0));
        result.setSteps(steps);
        result.setExecutionTime(high_resolution_clock::now() - startTime);
        return result;
    }

    QSet<QPair<int, int>> drawnPixels;

    auto addPixel = [&](int x, int y) {
        QPair<int, int> pixel(x, y);
        if (!drawnPixels.contains(pixel)) {
            drawnPixels.insert(pixel);
            addUniqueStep(steps, createStep(x, y, QString("R=%1").arg(radius), 1.0));
        }
    };

    int x = 0;
    int y = radius;
    int decisionParameter = 3 - 2 * radius;

    while (x <= y) {
        // Draw all eight symmetric points
        addPixel(center.x() + x, center.y() + y);
        addPixel(center.x() - x, center.y() + y);
        addPixel(center.x() + x, center.y() - y);
        addPixel(center.x() - x, center.y() - y);
        addPixel(center.x() + y, center.y() + x);
        addPixel(center.x() - y, center.y() + x);
        addPixel(center.x() + y, center.y() - x);
        addPixel(center.x() - y, center.y() - x);

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

AlgorithmResult RasterAlgorithms::kastlPitvey(const Point &start, const Point &end) {
    AlgorithmResult result;
    auto startTime = high_resolution_clock::now();

    int x0 = start.x(), y0 = start.y();
    int x1 = end.x(), y1 = end.y();

    QVector<AlgorithmStep> steps;

    if (x0 == x1 && y0 == y1) {
        steps.append(createStep(x0, y0, "Single point", 1.0));
        result.setSteps(steps);
        result.setExecutionTime(high_resolution_clock::now() - startTime);
        return result;
    }

    int ax = x0, ay = y0, bx = x1, by = y1;
    if (bx < ax) { std::swap(ax, bx); std::swap(ay, by); }

    int dx = bx - ax;
    int dy = by - ay;

    bool swapped_xy = false;
    bool reflect_x = false;
    bool reflect_y = false;

    if (dy < 0) { ay = -ay; by = -by; dy = -dy; reflect_y = true; }
    if (dx < 0) { ax = -ax; bx = -bx; dx = -dx; reflect_x = true; }
    if (dy > dx) { std::swap(ax, ay); std::swap(bx, by); std::swap(dx, dy); swapped_xy = true; }

    QVector<int> lowerY; lowerY.reserve(dx+1);
    QVector<int> upperY; upperY.reserve(dx+1);
    for (int xi = 0; xi <= dx; ++xi) {
        double xr = ax + xi;
        double yf = ay + (dx == 0 ? 0.0 : double(dy) * double(xr - ax) / double(dx));
        int lf = int(std::floor(yf + 1e-9));
        int uf = int(std::ceil(yf - 1e-9));
        lowerY.append(lf);
        upperY.append(uf);
    }

    QString route;
    int ycur = lowerY[0];
    for (int i = 0; i < dx; ++i) {
        bool canS = (ycur >= lowerY[i+1] && ycur <= upperY[i+1]);
        bool canD = (ycur + 1 >= lowerY[i+1] && ycur + 1 <= upperY[i+1]);
        double xr = ax + i + 1;
        double yf = ay + (dx == 0 ? 0.0 : double(dy) * double(xr - ax) / double(dx));
        if (canS && !canD) { route.push_back('S'); }
        else if (!canS && canD) { route.push_back('D'); ++ycur; }
        else {
            if (std::abs(ycur - yf) <= std::abs((ycur+1) - yf)) route.push_back('S');
            else { route.push_back('D'); ++ycur; }
        }
    }

    QVector<QPoint> pixels;
    pixels.reserve(route.size() + 1);
    int tx = ax;
    int ty = int(std::round(double(ay)));
    pixels.append(QPoint(tx, ty));
    int ycur2 = ty;
    for (int i = 0; i < route.size(); ++i) {
        char c = route[i].toLatin1();
        if (c == 'S') { tx += 1; }
        else { tx += 1; ycur2 += 1; }
        if (pixels.isEmpty() || pixels.last() != QPoint(tx, ycur2))
            pixels.append(QPoint(tx, ycur2));
    }

    QVector<AlgorithmStep> outSteps;
    outSteps.reserve(pixels.size());
    for (QPoint p : pixels) {
        int rx = p.x(), ry = p.y();
        if (swapped_xy) std::swap(rx, ry);
        if (reflect_x) rx = -rx;
        if (reflect_y) ry = -ry;
        if (!outSteps.isEmpty()) {
            Point prev = outSteps.last().getCoordinates();
            if (prev.x() == rx && prev.y() == ry) continue;
        }
        outSteps.append(createStep(rx, ry, QString("route:%1").arg(route), 1.0));
    }

    result.setSteps(outSteps);
    result.setExecutionTime(high_resolution_clock::now() - startTime);
    return result;
}


AlgorithmResult RasterAlgorithms::wuAntiAliasedLine(const Point &start, const Point &end) {
    AlgorithmResult result;
    auto startTime = high_resolution_clock::now();

    double x0 = start.x(), y0 = start.y();
    double x1 = end.x(), y1 = end.y();

    if (std::abs(x1 - x0) < 1e-6 && std::abs(y1 - y0) < 1e-6) {
        QVector<AlgorithmStep> steps;
        steps.append(createStep(int(x0), int(y0), "Single point", 1.0));
        result.setSteps(steps);
        result.setExecutionTime(high_resolution_clock::now() - startTime);
        return result;
    }

    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    double dx = x1 - x0;
    double dy = y1 - y0;
    double gradient = (dx == 0.0) ? 1.0 : dy / dx;

    QVector<AlgorithmStep> steps;

    // Helper function to plot with intensity
    auto plot = [&](double x, double y, double intensity, const QString &note = "") {
        if (intensity <= 0.0) return;

        int px, py;
        if (steep) {
            px = int(std::round(y));
            py = int(std::round(x));
        } else {
            px = int(std::round(x));
            py = int(std::round(y));
        }

        // Clamp intensity to [0, 1]
        intensity = std::max(0.0, std::min(1.0, intensity));

        // Check if this pixel already exists
        bool found = false;
        for (AlgorithmStep &s : steps) {
            Point p = s.getCoordinates();
            if (p.x() == px && p.y() == py) {
                // Sum intensities for the same pixel
                s.setIntensity(std::min(1.0, s.getIntensity() + intensity));
                found = true;
                break;
            }
        }

        if (!found) {
            steps.append(createStep(px, py, note, intensity));
        }
    };

    // First endpoint
    double xend = std::round(x0);
    double yend = y0 + gradient * (xend - x0);
    double xgap = reverseFractionalPart(x0 + 0.5);
    int xpxl1 = int(xend);
    int ypxl1 = int(std::floor(yend));

    plot(xpxl1, ypxl1, reverseFractionalPart(yend) * xgap, "first main");
    plot(xpxl1, ypxl1 + 1, fractionalPart(yend) * xgap, "first secondary");

    double intery = yend + gradient;

    // Second endpoint
    xend = std::round(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = fractionalPart(x1 + 0.5);
    int xpxl2 = int(xend);
    int ypxl2 = int(std::floor(yend));

    plot(xpxl2, ypxl2, reverseFractionalPart(yend) * xgap, "last main");
    plot(xpxl2, ypxl2 + 1, fractionalPart(yend) * xgap, "last secondary");

    // Main loop
    for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x) {
        plot(x, std::floor(intery), reverseFractionalPart(intery), QString("x=%1 lower").arg(x));
        plot(x, std::floor(intery) + 1, fractionalPart(intery), QString("x=%1 upper").arg(x));
        intery += gradient;
    }

    // Sort steps by coordinates for consistent output
    std::sort(steps.begin(), steps.end(),
              [](const AlgorithmStep &a, const AlgorithmStep &b) {
                  Point pa = a.getCoordinates();
                  Point pb = b.getCoordinates();
                  if (pa.x() != pb.x()) return pa.x() < pb.x();
                  return pa.y() < pb.y();
              });

    result.setSteps(steps);
    result.setExecutionTime(high_resolution_clock::now() - startTime);
    return result;
}
