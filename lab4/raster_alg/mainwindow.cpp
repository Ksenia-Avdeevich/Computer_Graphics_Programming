#include "mainwindow.h"
#include "canvaswidget.h"
#include "rasteralgorithms.h"
#include "report.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), currentAlgorithm(0) {
    setupInterface();
}

void MainWindow::setupInterface() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    canvas = new CanvasWidget(this);
    canvas->setMinimumSize(720, 520);

    executeBtn = new QPushButton(tr("Выполнить"), this);
    stepBtn = new QPushButton(tr("Следующий шаг"), this);
    clearBtn = new QPushButton(tr("Очистить"), this);
    exportBtn = new QPushButton(tr("Экспорт отчета"), this);

    algorithmSelector = new QComboBox(this);
    algorithmSelector->addItem("Пошаговый алгоритм");
    algorithmSelector->addItem("Цифровой дифференциальный анализатор (ЦДА)");
    algorithmSelector->addItem("Алгоритм Брезенхема (линия)");
    algorithmSelector->addItem("Алгоритм Брезенхема (окружность)");
    algorithmSelector->addItem("Алгоритм Ву (сглаживание)");
    algorithmSelector->addItem("Алгоритм Кастла-Питвея");

    pointAXSpin = new QSpinBox(this);
    pointAYSpin = new QSpinBox(this);
    pointBXSpin = new QSpinBox(this);
    pointBYSpin = new QSpinBox(this);

    pointAXSpin->setRange(-2000, 2000);
    pointAYSpin->setRange(-2000, 2000);
    pointBXSpin->setRange(-2000, 2000);
    pointBYSpin->setRange(-2000, 2000);

    pointAXSpin->setValue(0);
    pointAYSpin->setValue(0);
    pointBXSpin->setValue(0);
    pointBYSpin->setValue(0);

    showCalculationsCheck = new QCheckBox(tr("Показать вычисления"), this);
    logOutput = new QTextEdit(this);
    logOutput->setReadOnly(true);
    statusInfo = new QLabel(tr("Готов"), this);

    QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->addWidget(new QLabel("Алгоритм:"));
    controlLayout->addWidget(algorithmSelector);
    controlLayout->addWidget(new QLabel("Точка A (x,y):"));
    controlLayout->addWidget(pointAXSpin);
    controlLayout->addWidget(pointAYSpin);
    controlLayout->addWidget(new QLabel("Точка B (x,y) / Радиус:"));
    controlLayout->addWidget(pointBXSpin);
    controlLayout->addWidget(pointBYSpin);
    controlLayout->addWidget(showCalculationsCheck);
    controlLayout->addWidget(executeBtn);
    controlLayout->addWidget(stepBtn);
    controlLayout->addWidget(clearBtn);
    controlLayout->addWidget(exportBtn);
    controlLayout->addWidget(new QLabel("Журнал выполнения:"));
    controlLayout->addWidget(logOutput);
    controlLayout->addWidget(statusInfo);
    controlLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(canvas, 1);
    mainLayout->addLayout(controlLayout);

    connect(executeBtn, &QPushButton::clicked, this, &MainWindow::executeAlgorithm);
    connect(stepBtn, &QPushButton::clicked, this, &MainWindow::stepForward);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearCanvas);
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::exportReport);
    connect(algorithmSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::algorithmSelectionChanged);

    connect(canvas, &CanvasWidget::pointASelected, this, &MainWindow::setPointACoordinates);
    connect(canvas, &CanvasWidget::pointBSelected, this, &MainWindow::setPointBCoordinates);

    connect(showCalculationsCheck, &QCheckBox::toggled, this, [this](bool enabled){
        canvas->setCalculationDisplay(enabled);
        if (enabled) {
            const auto &steps = canvas->getAlgorithmSteps();
            logOutput->clear();
            for (int i = 0; i < steps.size(); ++i) {
                Point p = steps[i].getCoordinates();
                QString intensityInfo = "";
                if (algorithmSelector->currentIndex() == 4) { // Алгоритм Ву
                    intensityInfo = QString(" Интенсивность: %1").arg(steps[i].getIntensity(), 0, 'f', 2);
                }
                logOutput->append(QString("Шаг %1: (%2, %3) %4%5")
                                      .arg(i).arg(p.x()).arg(p.y()).arg(steps[i].getDescription()).arg(intensityInfo));
            }
        }
    });
}

void MainWindow::executeAlgorithm() {
    logOutput->clear();
    currentAlgorithm = algorithmSelector->currentIndex();
    Point A(pointAXSpin->value(), pointAYSpin->value());
    Point B(pointBXSpin->value(), pointBYSpin->value());

    AlgorithmResult result;

    switch (currentAlgorithm) {
    case 0: result = RasterAlgorithms::stepByStep(A, B); break;
    case 1: result = RasterAlgorithms::digitalDifferentialAnalyzer(A, B); break;
    case 2: result = RasterAlgorithms::bresenhamLine(A, B); break;
    case 3: result = RasterAlgorithms::bresenhamCircle(A, B); break;
    case 4: result = RasterAlgorithms::wuAntiAliasedLine(A, B); break;
    case 5: result = RasterAlgorithms::kastlPitvey(A, B); break;
    }

    canvas->setAlgorithmSteps(result.getSteps());
    canvas->resetStepCounter();
    canvas->update();

    logOutput->append(QString("Алгоритм: %1").arg(algorithmSelector->currentText()));
    logOutput->append(QString("Всего шагов: %1").arg(result.getSteps().size()));
    logOutput->append(QString("Время выполнения: %1 мс").arg(result.getExecutionTime().count() * 1000.0, 0, 'f', 3));

    // Специальная информация для алгоритма Ву
    if (currentAlgorithm == 4) {
        logOutput->append("\n=== Алгоритм Ву (сглаживание) ===");
        logOutput->append("Этот алгоритм использует значения интенсивности для создания гладких линий.");
        logOutput->append("Диапазон интенсивности: 0.0 (прозрачный) до 1.0 (непрозрачный)");
        logOutput->append("Интенсивность каждого пикселя зависит от его расстояния до идеальной линии.");
    }

    statusInfo->setText("Алгоритм выполнен успешно");

    if (showCalculationsCheck->isChecked()) {
        logOutput->append("\nПодробная информация о шагах:");
        const auto &steps = result.getSteps();
        for (int i = 0; i < steps.size(); ++i) {
            Point p = steps[i].getCoordinates();
            QString description = steps[i].getDescription();
            QString intensityInfo = "";
            if (currentAlgorithm == 4) {
                intensityInfo = QString(" [Интенсивность: %1]").arg(steps[i].getIntensity(), 0, 'f', 2);
            }
            logOutput->append(QString("Шаг %1: (%2, %3) %4%5").arg(i).arg(p.x()).arg(p.y()).arg(description).arg(intensityInfo));
        }
    }
}

void MainWindow::stepForward() {
    canvas->advanceStep();
}

void MainWindow::clearCanvas() {
    canvas->clear();
    logOutput->clear();
    statusInfo->setText("Холст очищен");
}

void MainWindow::exportReport() {
    Report report = Report::fromCanvas(canvas);
    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранить отчет"), QString(), tr("Текстовые файлы (*.txt)"));
    if (filename.isEmpty()) return;

    if (!report.saveToFile(filename)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось сохранить отчет"));
    } else {
        statusInfo->setText("Отчет успешно экспортирован");
    }
}

void MainWindow::algorithmSelectionChanged(int index) {
    // Обновление подсказок в зависимости от выбранного алгоритма
    if (index == 4) { // Алгоритм Ву
        statusInfo->setText("Алгоритм Ву: создает гладкие сглаженные линии с использованием значений интенсивности");
    } else if (index == 5) { // Алгоритм Кастла-Питвея
        statusInfo->setText("Алгоритм Кастла-Питвея: оптимальная 8-связная цепочка пикселей с субпиксельной точностью");
    } else {
        statusInfo->setText("Готов");
    }
}
void MainWindow::setPointACoordinates() {
    QPoint point = canvas->getLastWorldClick();
    pointAXSpin->setValue(point.x());
    pointAYSpin->setValue(point.y());
}

void MainWindow::setPointBCoordinates() {
    QPoint point = canvas->getLastWorldClick();
    pointBXSpin->setValue(point.x());
    pointBYSpin->setValue(point.y());
}
