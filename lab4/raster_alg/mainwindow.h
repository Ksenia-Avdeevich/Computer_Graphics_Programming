#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CanvasWidget;
class QPushButton;
class QComboBox;
class QSpinBox;
class QLabel;
class QTextEdit;
class QCheckBox;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void executeAlgorithm();
    void stepForward();
    void clearCanvas();
    void exportReport();
    void algorithmSelectionChanged(int index);
    void setPointACoordinates();
    void setPointBCoordinates();

private:
    void setupInterface();
    CanvasWidget *canvas;
    QPushButton *executeBtn;
    QPushButton *stepBtn;
    QPushButton *clearBtn;
    QPushButton *exportBtn;
    QComboBox *algorithmSelector;
    QSpinBox *pointAXSpin;
    QSpinBox *pointAYSpin;
    QSpinBox *pointBXSpin;
    QSpinBox *pointBYSpin;
    QTextEdit *logOutput;
    QCheckBox *showCalculationsCheck;
    QLabel *statusInfo;
    int currentAlgorithm;
};

#endif
