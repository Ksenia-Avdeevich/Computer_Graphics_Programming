#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QWidget>
#include <QVector>
#include <QVector3D>
#include <QPair>
#include "matrix4x4.h"

class DrawingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingWidget(QWidget *parent = nullptr);

    void setVertices(const QVector<QVector3D>& v) { vertices = v; }
    void setEdges(const QVector<QPair<int, int>>& e) { edges = e; }
    void setTransformMatrix(const Matrix4x4& m) { transformMatrix = m; }
    void setProjectionType(int type) { projectionType = type; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QVector3D> vertices;
    QVector<QPair<int, int>> edges;
    Matrix4x4 transformMatrix;
    int projectionType;

    void drawAxes(QPainter& painter, int centerX, int centerY);
    void drawObject(QPainter& painter, int centerX, int centerY);
    QPoint project3DTo2D(const QVector3D& point, int centerX, int centerY);
};

#endif // DRAWINGWIDGET_H
