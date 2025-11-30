#ifndef REPORT_H
#define REPORT_H

#include <QString>
#include <QVector>

class AlgorithmStep;
class CanvasWidget;

class Report {
public:
    Report();

    void setTitle(const QString &title);
    QString getTitle() const;

    void setSteps(const QVector<AlgorithmStep> &steps);
    QVector<AlgorithmStep> getSteps() const;

    void setExecutionTime(double time);
    double getExecutionTime() const;

    QString generateReport() const;
    bool saveToFile(const QString &filepath) const;

    static Report fromCanvas(const CanvasWidget *canvas);

private:
    QString reportTitle;
    QVector<AlgorithmStep> algorithmSteps;
    double executionTimeMs;
};

#endif
