#include "canvaswidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent),
    currentStepIndex(-1),
    zoomLevel(24.0),
    panOffset(0,0) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
}

void CanvasWidget::setAlgorithmSteps(const QVector<AlgorithmStep> &steps) {
    algorithmSteps = steps;
    currentStepIndex = steps.isEmpty() ? -1 : steps.size() - 1;
    update();
}

void CanvasWidget::clear() {
    algorithmSteps.clear();
    currentStepIndex = -1;
    update();
}

void CanvasWidget::advanceStep() {
    if (algorithmSteps.isEmpty()) return;
    if (currentStepIndex < int(algorithmSteps.size()) - 1) {
        ++currentStepIndex;
    }
    update();
}

void CanvasWidget::resetStepCounter() {
    currentStepIndex = -1;
    update();
}

QPoint CanvasWidget::getLastWorldClick() const {
    return lastWorldClick;
}

const QVector<AlgorithmStep>& CanvasWidget::getAlgorithmSteps() const {
    return algorithmSteps;
}

QPoint CanvasWidget::convertToScreen(const QPoint &world) const {
    int screenX = int(width() / 2 + (world.x() + panOffset.x()) * zoomLevel);
    int screenY = int(height() / 2 - (world.y() + panOffset.y()) * zoomLevel);
    return QPoint(screenX, screenY);
}

QPoint CanvasWidget::convertToWorld(const QPoint &screen) const {
    double worldX = (screen.x() - width() / 2) / zoomLevel - panOffset.x();
    double worldY = (height() / 2 - screen.y()) / zoomLevel - panOffset.y();
    return QPoint(int(std::round(worldX)), int(std::round(worldY)));
}

void CanvasWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.fillRect(rect(), Qt::white);

    // Draw grid
    painter.setPen(QPen(QColor(230, 230, 230)));
    int gridStep = int(std::round(zoomLevel));
    QPoint center(width() / 2, height() / 2);

    if (gridStep > 0) {
        for (int x = center.x() % gridStep; x < width(); x += gridStep) {
            painter.drawLine(x, 0, x, height());
        }
        for (int y = center.y() % gridStep; y < height(); y += gridStep) {
            painter.drawLine(0, y, width(), y);
        }
    }

    // Draw coordinate axes
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(0, center.y(), width(), center.y());
    painter.drawLine(center.x(), 0, center.x(), height());

    // Draw axis ticks and labels
    const int tickSize = 4;
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    painter.setPen(Qt::black);

    int xCount = width() / gridStep / 2 + 2;
    for (int i = -xCount; i <= xCount; ++i) {
        int x = center.x() + i * gridStep;
        painter.drawLine(x, center.y() - tickSize, x, center.y() + tickSize);
        if (i != 0) {
            QString label = QString::number(i);
            QRect textRect(x - 12, center.y() + 6, 24, 12);
            painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, label);
        }
    }

    int yCount = height() / gridStep / 2 + 2;
    for (int j = -yCount; j <= yCount; ++j) {
        int y = center.y() - j * gridStep;
        painter.drawLine(center.x() - tickSize, y, center.x() + tickSize, y);
        if (j != 0) {
            QString label = QString::number(j);
            QRect textRect(center.x() + 6, y - 8, 24, 12);
            painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, label);
        }
    }

    // Draw axis labels
    painter.drawText(width() - 20, center.y() - 10, "X");
    painter.drawText(center.x() + 6, 15, "Y");

    // Draw algorithm steps
    for (int i = 0; i < algorithmSteps.size(); ++i) {
        Point point = algorithmSteps[i].getCoordinates();
        double intensity = algorithmSteps[i].getIntensity();
        QPoint screenPos = convertToScreen(QPoint(point.x(), point.y()));
        int pixelSize = std::max(2, int(std::round(zoomLevel)));
        QRect pixelRect(screenPos.x() - pixelSize / 2, screenPos.y() - pixelSize / 2, pixelSize, pixelSize);

        QColor baseColor = (i <= currentStepIndex) ? QColor(50, 120, 200) : QColor(180, 200, 240);
        painter.fillRect(pixelRect, Qt::white);

        QColor fillColor = baseColor;
        int alpha = int(qBound(0, int(std::round(intensity * 255.0)), 255));
        fillColor.setAlpha(alpha);

        painter.save();
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.fillRect(pixelRect, fillColor);
        painter.restore();

        painter.setPen(Qt::black);
        painter.drawRect(pixelRect);
    }

    // Highlight current step
    if (currentStepIndex >= 0 && currentStepIndex < algorithmSteps.size()) {
        Point currentPoint = algorithmSteps[currentStepIndex].getCoordinates();
        QPoint screenPos = convertToScreen(QPoint(currentPoint.x(), currentPoint.y()));
        painter.setPen(QPen(Qt::red, 2));
        int highlightSize = int(zoomLevel) + 4;
        painter.drawRect(screenPos.x() - highlightSize / 2, screenPos.y() - highlightSize / 2,
                         highlightSize, highlightSize);

        if (showCalculations) {
            painter.drawText(screenPos.x() + highlightSize / 2 + 2, screenPos.y(),
                             algorithmSteps[currentStepIndex].getDescription());
        }
    }
}

void CanvasWidget::wheelEvent(QWheelEvent *event) {
    double delta = event->angleDelta().y() / 120.0;
    double zoomFactor = std::pow(1.2, delta);
    zoomLevel *= zoomFactor;

    // Limit zoom range
    if (zoomLevel < 6) zoomLevel = 6;
    if (zoomLevel > 240) zoomLevel = 240;

    update();
}

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastWorldClick = convertToWorld(event->pos());
        emit pointASelected();
    } else if (event->button() == Qt::RightButton) {
        lastWorldClick = convertToWorld(event->pos());
        emit pointBSelected();
    } else if (event->button() == Qt::MiddleButton) {
        panOffset = QPointF(0, 0);
        update();
    }
}
