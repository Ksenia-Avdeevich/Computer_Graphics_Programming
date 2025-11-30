#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QPoint>

class Point {
public:
    Point() : xCoord(0), yCoord(0) {}
    Point(int x, int y) : xCoord(x), yCoord(y) {}
    int x() const { return xCoord; }
    int y() const { return yCoord; }
    void setX(int value) { xCoord = value; }
    void setY(int value) { yCoord = value; }

private:
    int xCoord;
    int yCoord;
};

class AlgorithmStep {
public:
    AlgorithmStep() : coordinates(), description(), intensityValue(1.0) {}
    AlgorithmStep(const Point &point, const QString &desc = QString(), double intensity = 1.0)
        : coordinates(point), description(desc), intensityValue(intensity) {}

    Point getCoordinates() const { return coordinates; }
    QString getDescription() const { return description; }
    double getIntensity() const { return intensityValue; }

    void setCoordinates(const Point &point) { coordinates = point; }
    void setDescription(const QString &desc) { description = desc; }
    void setIntensity(double value) { intensityValue = qBound(0.0, value, 1.0); }

private:
    Point coordinates;
    QString description;
    double intensityValue;
};

class CanvasWidget : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);
    void setAlgorithmSteps(const QVector<AlgorithmStep>& steps);
    void clear();
    void advanceStep();
    void resetStepCounter();
    QPoint getLastWorldClick() const;
    const QVector<AlgorithmStep>& getAlgorithmSteps() const;
    void setCalculationDisplay(bool display) { showCalculations = display; update(); }
    bool isCalculationDisplayed() const { return showCalculations; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPoint convertToScreen(const QPoint &world) const;
    QPoint convertToWorld(const QPoint &screen) const;

    QVector<AlgorithmStep> algorithmSteps;
    int currentStepIndex;
    double zoomLevel;
    QPointF panOffset;
    QPoint lastWorldClick;
    bool showCalculations = false;

signals:
    void pointASelected();
    void pointBSelected();
};

#endif
