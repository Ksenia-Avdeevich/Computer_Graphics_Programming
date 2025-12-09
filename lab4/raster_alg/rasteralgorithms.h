#ifndef RASTERALGORITHMS_H
#define RASTERALGORITHMS_H

#include <QVector>
#include <QString>
#include <chrono>
#include "canvaswidget.h"

class AlgorithmResult {
public:
    AlgorithmResult() : executionTime(std::chrono::duration<double>::zero()) {}


    void setSteps(const QVector<AlgorithmStep>& steps) { algorithmSteps = steps; }
    const QVector<AlgorithmStep>& getSteps() const { return algorithmSteps; }

    void setExecutionTime(const std::chrono::duration<double> &time) { executionTime = time; }
    std::chrono::duration<double> getExecutionTime() const { return executionTime; }

private:
    QVector<AlgorithmStep> algorithmSteps;
    std::chrono::duration<double> executionTime;
};

class RasterAlgorithms {
public:
    static AlgorithmResult kastlPitvey(const Point &start, const Point &end);
    static AlgorithmResult stepByStep(const Point &start, const Point &end);
    static AlgorithmResult digitalDifferentialAnalyzer(const Point &start, const Point &end);
    static AlgorithmResult bresenhamLine(const Point &start, const Point &end);
    static AlgorithmResult bresenhamCircle(const Point &center, const Point &radiusPoint);
    static AlgorithmResult wuAntiAliasedLine(const Point &start, const Point &end);
};

#endif
