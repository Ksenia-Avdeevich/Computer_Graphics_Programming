#include "mainwindow.h"
#include "drawingwidget.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QSpacerItem>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , drawingArea(nullptr)
{
    setWindowTitle("Лаб 6: 3D Буква A");
    resize(1200, 800);

    initializeLetterA();
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Центральный виджет
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    // Главный layout - горизонтальный
    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    // Левая панель управления
    QWidget* controlPanel = new QWidget();
    controlPanel->setMaximumWidth(320);
    controlPanel->setMinimumWidth(320);
    QVBoxLayout* controlLayout = new QVBoxLayout(controlPanel);

    // Заголовок
    QLabel* titleLabel = new QLabel("Управление");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    controlLayout->addWidget(titleLabel);

    // Группа "Перенос"
    QGroupBox* translateGroup = new QGroupBox("Перенос");
    QFormLayout* translateLayout = new QFormLayout(translateGroup);

    spinTx = new QDoubleSpinBox();
    spinTx->setRange(-10.0, 10.0);
    spinTx->setSingleStep(0.5);
    spinTx->setValue(0.0);

    spinTy = new QDoubleSpinBox();
    spinTy->setRange(-10.0, 10.0);
    spinTy->setSingleStep(0.5);
    spinTy->setValue(0.0);

    spinTz = new QDoubleSpinBox();
    spinTz->setRange(-10.0, 10.0);
    spinTz->setSingleStep(0.5);
    spinTz->setValue(0.0);

    QPushButton* btnTranslate = new QPushButton("Применить");

    translateLayout->addRow("X:", spinTx);
    translateLayout->addRow("Y:", spinTy);
    translateLayout->addRow("Z:", spinTz);
    translateLayout->addRow(btnTranslate);

    controlLayout->addWidget(translateGroup);

    // Группа "Масштабирование"
    QGroupBox* scaleGroup = new QGroupBox("Масштабирование");
    QFormLayout* scaleLayout = new QFormLayout(scaleGroup);

    spinSx = new QDoubleSpinBox();
    spinSx->setRange(0.1, 5.0);
    spinSx->setSingleStep(0.1);
    spinSx->setValue(1.0);

    spinSy = new QDoubleSpinBox();
    spinSy->setRange(0.1, 5.0);
    spinSy->setSingleStep(0.1);
    spinSy->setValue(1.0);

    spinSz = new QDoubleSpinBox();
    spinSz->setRange(0.1, 5.0);
    spinSz->setSingleStep(0.1);
    spinSz->setValue(1.0);

    QPushButton* btnScale = new QPushButton("Применить");

    scaleLayout->addRow("X:", spinSx);
    scaleLayout->addRow("Y:", spinSy);
    scaleLayout->addRow("Z:", spinSz);
    scaleLayout->addRow(btnScale);

    controlLayout->addWidget(scaleGroup);

    // Группа "Вращение"
    QGroupBox* rotateGroup = new QGroupBox("Вращение");
    QFormLayout* rotateLayout = new QFormLayout(rotateGroup);

    spinAngle = new QDoubleSpinBox();
    spinAngle->setRange(-360.0, 360.0);
    spinAngle->setSingleStep(5.0);
    spinAngle->setValue(0.0);

    spinAx = new QDoubleSpinBox();
    spinAx->setRange(-1.0, 1.0);
    spinAx->setSingleStep(0.1);
    spinAx->setValue(0.0);

    spinAy = new QDoubleSpinBox();
    spinAy->setRange(-1.0, 1.0);
    spinAy->setSingleStep(0.1);
    spinAy->setValue(0.0);

    spinAz = new QDoubleSpinBox();
    spinAz->setRange(-1.0, 1.0);
    spinAz->setSingleStep(0.1);
    spinAz->setValue(0.0);

    QPushButton* btnRotate = new QPushButton("Применить");

    rotateLayout->addRow("Угол:", spinAngle);
    rotateLayout->addRow("Ось X:", spinAx);
    rotateLayout->addRow("Ось Y:", spinAy);
    rotateLayout->addRow("Ось Z:", spinAz);
    rotateLayout->addRow(btnRotate);

    controlLayout->addWidget(rotateGroup);

    // Группа "Проекция"
    QGroupBox* projectionGroup = new QGroupBox("Проекция");
    QVBoxLayout* projectionLayout = new QVBoxLayout(projectionGroup);

    comboProjection = new QComboBox();
    comboProjection->addItem("Перспективная");
    comboProjection->addItem("Ортогональная XY");
    comboProjection->addItem("Ортогональная XZ");
    comboProjection->addItem("Ортогональная YZ");

    projectionLayout->addWidget(comboProjection);
    controlLayout->addWidget(projectionGroup);

    // Кнопка сброса
    QPushButton* btnReset = new QPushButton("Сброс");
    controlLayout->addWidget(btnReset);

    // Группа "Матрица преобразования"
    QGroupBox* matrixGroup = new QGroupBox("Матрица преобразования");
    QVBoxLayout* matrixLayout = new QVBoxLayout(matrixGroup);

    textMatrix = new QTextEdit();
    textMatrix->setReadOnly(true);
    textMatrix->setMaximumHeight(150);
    QFont monoFont("Courier New", 9);
    textMatrix->setFont(monoFont);

    matrixLayout->addWidget(textMatrix);
    controlLayout->addWidget(matrixGroup);

    // Пружина внизу
    controlLayout->addStretch();

    // Правая область - область рисования
    drawingArea = new DrawingWidget();

    // Добавляем в главный layout
    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(drawingArea, 1);

    // Подключение сигналов
    connect(btnTranslate, &QPushButton::clicked, this, &MainWindow::onTranslate);
    connect(btnScale, &QPushButton::clicked, this, &MainWindow::onScale);
    connect(btnRotate, &QPushButton::clicked, this, &MainWindow::onRotate);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(comboProjection, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onProjectionChanged);

    // Инициализируем матрицу и рисунок
    displayMatrix();
    updateDrawing();
}

void MainWindow::initializeLetterA()
{
    vertices.clear();
    edges.clear();

    // Буква A - более реалистичная модель
    // Передняя грань (z = 0)

    // Левая ножка (от низа к верху)
    vertices << QVector3D(-2.0, -3.0, 0);   // 0 - низ левой ноги (внешний)
    vertices << QVector3D(-1.5, -3.0, 0);   // 1 - низ левой ноги (внутренний)
    vertices << QVector3D(-0.2, 3.0, 0);    // 2 - верх левой ноги (внутренний)
    vertices << QVector3D(-0.7, 3.0, 0);    // 3 - верх левой ноги (внешний)

    // Правая ножка (от низа к верху)
    vertices << QVector3D(1.5, -3.0, 0);    // 4 - низ правой ноги (внутренний)
    vertices << QVector3D(2.0, -3.0, 0);    // 5 - низ правой ноги (внешний)
    vertices << QVector3D(0.7, 3.0, 0);     // 6 - верх правой ноги (внешний)
    vertices << QVector3D(0.2, 3.0, 0);     // 7 - верх правой ноги (внутренний)

    // Перекладина
    vertices << QVector3D(-0.8, 0.0, 0);    // 8 - левая перекладина (низ)
    vertices << QVector3D(-0.5, 0.8, 0);    // 9 - левая перекладина (верх)
    vertices << QVector3D(0.5, 0.8, 0);     // 10 - правая перекладина (верх)
    vertices << QVector3D(0.8, 0.0, 0);     // 11 - правая перекладина (низ)

    // Задняя грань (z = 0.8) - копия передней грани
    int frontCount = vertices.size();
    for(int i = 0; i < frontCount; i++) {
        QVector3D v = vertices[i];
        vertices << QVector3D(v.x(), v.y(), 0.8);
    }

    // Рёбра передней грани
    // Левая нога (контур)
    edges << qMakePair(0, 1);   // низ левой ноги
    edges << qMakePair(1, 2);   // внутренняя сторона левой ноги
    edges << qMakePair(2, 3);   // верх левой ноги
    edges << qMakePair(3, 0);   // внешняя сторона левой ноги

    // Правая нога (контур)
    edges << qMakePair(4, 5);   // низ правой ноги
    edges << qMakePair(5, 6);   // внешняя сторона правой ноги
    edges << qMakePair(6, 7);   // верх правой ноги
    edges << qMakePair(7, 4);   // внутренняя сторона правой ноги

    // Перекладина (контур)
    edges << qMakePair(8, 9);   // левая сторона перекладины
    edges << qMakePair(9, 10);  // верх перекладины
    edges << qMakePair(10, 11); // правая сторона перекладины
    edges << qMakePair(11, 8);  // низ перекладины

    // Соединения (где перекладина встречается с ножками)
    edges << qMakePair(2, 9);   // левая нога к перекладине
    edges << qMakePair(7, 10);  // правая нога к перекладине

    // Рёбра задней грани (аналогично передней)
    edges << qMakePair(12, 13);
    edges << qMakePair(13, 14);
    edges << qMakePair(14, 15);
    edges << qMakePair(15, 12);

    edges << qMakePair(16, 17);
    edges << qMakePair(17, 18);
    edges << qMakePair(18, 19);
    edges << qMakePair(19, 16);

    edges << qMakePair(20, 21);
    edges << qMakePair(21, 22);
    edges << qMakePair(22, 23);
    edges << qMakePair(23, 20);

    edges << qMakePair(14, 21);
    edges << qMakePair(19, 22);

    // Соединяем переднюю и заднюю грани
    for(int i = 0; i < frontCount; i++) {
        edges << qMakePair(i, i + frontCount);
    }
}

void MainWindow::updateDrawing()
{
    if (drawingArea) {
        drawingArea->setVertices(vertices);
        drawingArea->setEdges(edges);
        drawingArea->setTransformMatrix(transformMatrix);
        drawingArea->update();
    }
}

Matrix4x4 MainWindow::getTranslationMatrix(double tx, double ty, double tz)
{
    Matrix4x4 mat;
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;
    return mat;
}

Matrix4x4 MainWindow::getScaleMatrix(double sx, double sy, double sz)
{
    Matrix4x4 mat;
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;
    return mat;
}

Matrix4x4 MainWindow::getRotationMatrix(double angle, double ax, double ay, double az)
{
    double len = sqrt(ax*ax + ay*ay + az*az);
    if(len < 0.0001) return Matrix4x4();

    ax /= len; ay /= len; az /= len;

    double rad = angle * M_PI / 180.0;
    double c = cos(rad);
    double s = sin(rad);
    double t = 1.0 - c;

    Matrix4x4 mat;
    mat.m[0][0] = t*ax*ax + c;
    mat.m[0][1] = t*ax*ay - s*az;
    mat.m[0][2] = t*ax*az + s*ay;

    mat.m[1][0] = t*ax*ay + s*az;
    mat.m[1][1] = t*ay*ay + c;
    mat.m[1][2] = t*ay*az - s*ax;

    mat.m[2][0] = t*ax*az - s*ay;
    mat.m[2][1] = t*ay*az + s*ax;
    mat.m[2][2] = t*az*az + c;

    return mat;
}

void MainWindow::onTranslate()
{
    double tx = spinTx->value();
    double ty = spinTy->value();
    double tz = spinTz->value();

    Matrix4x4 transMat = getTranslationMatrix(tx, ty, tz);
    transformMatrix = transMat * transformMatrix;

    displayMatrix();
    updateDrawing();
}

void MainWindow::onScale()
{
    double sx = spinSx->value();
    double sy = spinSy->value();
    double sz = spinSz->value();

    Matrix4x4 scaleMat = getScaleMatrix(sx, sy, sz);
    transformMatrix = scaleMat * transformMatrix;

    displayMatrix();
    updateDrawing();
}

void MainWindow::onRotate()
{
    double angle = spinAngle->value();
    double ax = spinAx->value();
    double ay = spinAy->value();
    double az = spinAz->value();

    Matrix4x4 rotMat = getRotationMatrix(angle, ax, ay, az);
    transformMatrix = rotMat * transformMatrix;

    displayMatrix();
    updateDrawing();
}

void MainWindow::onReset()
{
    transformMatrix = Matrix4x4();

    spinTx->setValue(0);
    spinTy->setValue(0);
    spinTz->setValue(0);

    spinSx->setValue(1);
    spinSy->setValue(1);
    spinSz->setValue(1);

    spinAngle->setValue(0);
    spinAx->setValue(0);
    spinAy->setValue(0);
    spinAz->setValue(0);

    displayMatrix();
    updateDrawing();
}

void MainWindow::onProjectionChanged(int index)
{
    if (drawingArea) {
        drawingArea->setProjectionType(index);
        drawingArea->update();
    }
}

void MainWindow::displayMatrix()
{
    QString matrixText;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            matrixText += QString("%1").arg(transformMatrix.m[i][j], 7, 'f', 3);
            if (j < 3) matrixText += "  ";
        }
        matrixText += "\n";
    }
    textMatrix->setText(matrixText);
}
