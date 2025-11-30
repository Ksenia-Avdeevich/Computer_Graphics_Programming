#include "report.h"
#include "canvaswidget.h"
#include <QFile>
#include <QTextStream>

Report::Report() : reportTitle(""), executionTimeMs(0.0) {}

void Report::setTitle(const QString &title) {
    reportTitle = title;
}

QString Report::getTitle() const {
    return reportTitle;
}

void Report::setSteps(const QVector<AlgorithmStep> &steps) {
    algorithmSteps = steps;
}

QVector<AlgorithmStep> Report::getSteps() const {
    return algorithmSteps;
}

void Report::setExecutionTime(double time) {
    executionTimeMs = time;
}

double Report::getExecutionTime() const {
    return executionTimeMs;
}

QString Report::generateReport() const {
    QString report;

    report += "=== Raster Algorithms Execution Report ===\n\n";
    report += "Title: " + reportTitle + "\n";
    report += QString("Number of steps: %1\n").arg(algorithmSteps.size());
    report += QString("Execution time: %1 ms\n").arg(executionTimeMs, 0, 'f', 3);
    report += "\nStep Details:\n";
    report += "-------------\n";

    for (int i = 0; i < algorithmSteps.size(); ++i) {
        Point point = algorithmSteps[i].getCoordinates();
        QString description = algorithmSteps[i].getDescription();
        report += QString("Step %1: (%2, %3) %4\n")
                      .arg(i)
                      .arg(point.x())
                      .arg(point.y())
                      .arg(description.isEmpty() ? "No description" : description);
    }

    return report;
}

bool Report::saveToFile(const QString &filepath) const {
    QFile file(filepath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }

    QTextStream stream(&file);
    stream << generateReport();
    file.close();

    return true;
}

Report Report::fromCanvas(const CanvasWidget *canvas) {
    Report report;
    report.setTitle("Algorithm Visualization Report");
    report.setSteps(canvas->getAlgorithmSteps());
    report.setExecutionTime(0.0); // This would need to be calculated during algorithm execution

    return report;
}
