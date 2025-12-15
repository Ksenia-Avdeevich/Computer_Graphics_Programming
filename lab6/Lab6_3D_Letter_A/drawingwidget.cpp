#include "drawingwidget.h"
#include <QPainter>
#include <cmath>

DrawingWidget::DrawingWidget(QWidget *parent)
    : QWidget(parent)
    , projectionType(0)
{
    setMinimumSize(600, 600);
    setStyleSheet("background-color: white;");
}

void DrawingWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Белый фон
    painter.fillRect(rect(), Qt::white);

    int centerX = width() / 2;
    int centerY = height() / 2;

    // Рисуем оси координат
    drawAxes(painter, centerX, centerY);

    // Рисуем объект
    if (!vertices.isEmpty() && !edges.isEmpty()) {
        drawObject(painter, centerX, centerY);
    }
}

void DrawingWidget::drawAxes(QPainter& painter, int centerX, int centerY)
{
    // Ось X (красная)
    painter.setPen(QPen(Qt::red, 3));
    painter.drawLine(centerX, centerY, centerX + 200, centerY);
    painter.drawText(centerX + 210, centerY + 5, "X");

    // Ось Y (зеленая)
    painter.setPen(QPen(Qt::green, 3));
    painter.drawLine(centerX, centerY, centerX, centerY - 200);
    painter.drawText(centerX - 10, centerY - 210, "Y");

    // Ось Z (синяя)
    painter.setPen(QPen(Qt::blue, 3));
    painter.drawLine(centerX, centerY, centerX - 100, centerY + 100);
    painter.drawText(centerX - 120, centerY + 115, "Z");

    // Сетка
    painter.setPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
    for (int i = -6; i <= 6; i++) {
        if (i == 0) continue;
        // Вертикальные линии
        painter.drawLine(centerX + i * 40, centerY - 240,
                         centerX + i * 40, centerY + 240);
        // Горизонтальные линии
        painter.drawLine(centerX - 240, centerY + i * 40,
                         centerX + 240, centerY + i * 40);
    }
}

void DrawingWidget::drawObject(QPainter& painter, int centerX, int centerY)
{
    // Рисуем рёбра
    painter.setPen(QPen(Qt::black, 2));

    for(const auto& edge : edges) {
        QVector3D v1 = transformMatrix.transform(vertices[edge.first]);
        QVector3D v2 = transformMatrix.transform(vertices[edge.second]);

        QPoint p1 = project3DTo2D(v1, centerX, centerY);
        QPoint p2 = project3DTo2D(v2, centerX, centerY);

        painter.drawLine(p1, p2);
    }

    // Рисуем вершины
    painter.setPen(QPen(Qt::red, 8));
    for(const auto& vertex : vertices) {
        QVector3D v = transformMatrix.transform(vertex);
        QPoint p = project3DTo2D(v, centerX, centerY);
        painter.drawPoint(p);
    }
}

QPoint DrawingWidget::project3DTo2D(const QVector3D& point, int centerX, int centerY)
{
    int scale = 70;

    switch(projectionType) {
    case 0: { // PERSPECTIVE
        double distance = 15.0;
        double factor = distance / (distance + point.z());
        int x = centerX + static_cast<int>(point.x() * scale * factor);
        int y = centerY - static_cast<int>(point.y() * scale * factor);
        return QPoint(x, y);
    }
    case 1: // ORTHO_XY
        return QPoint(centerX + point.x() * scale,
                      centerY - point.y() * scale);
    case 2: // ORTHO_XZ
        return QPoint(centerX + point.x() * scale,
                      centerY - point.z() * scale);
    case 3: // ORTHO_YZ
        return QPoint(centerX + point.z() * scale,
                      centerY - point.y() * scale);
    }

    return QPoint(centerX, centerY);
}
