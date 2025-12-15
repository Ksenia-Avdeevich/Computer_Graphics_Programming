#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QVector3D>
#include <QPair>
#include "matrix4x4.h"

// Forward declarations
class QDoubleSpinBox;
class QPushButton;
class QComboBox;
class QTextEdit;
class DrawingWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTranslate();
    void onScale();
    void onRotate();
    void onReset();
    void onProjectionChanged(int index);

private:
    // UI элементы
    DrawingWidget* drawingArea;
    QDoubleSpinBox* spinTx;
    QDoubleSpinBox* spinTy;
    QDoubleSpinBox* spinTz;
    QDoubleSpinBox* spinSx;
    QDoubleSpinBox* spinSy;
    QDoubleSpinBox* spinSz;
    QDoubleSpinBox* spinAngle;
    QDoubleSpinBox* spinAx;
    QDoubleSpinBox* spinAy;
    QDoubleSpinBox* spinAz;
    QComboBox* comboProjection;
    QTextEdit* textMatrix;

    // Вершины буквы A
    QVector<QVector3D> vertices;
    QVector<QPair<int, int>> edges;

    // Матрица преобразования
    Matrix4x4 transformMatrix;

    void setupUI();
    void initializeLetterA();
    void updateDrawing();
    Matrix4x4 getTranslationMatrix(double tx, double ty, double tz);
    Matrix4x4 getScaleMatrix(double sx, double sy, double sz);
    Matrix4x4 getRotationMatrix(double angle, double ax, double ay, double az);
    void displayMatrix();
};

#endif // MAINWINDOW_H
