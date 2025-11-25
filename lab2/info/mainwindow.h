#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include "imageinfo.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadImages();
    void onTableCellClicked(int row, int column);

private:
    QTableWidget *tableWidget;
    QPushButton *btnLoadImages;
    QLineEdit *folderPathEdit;
    QProgressBar *progressBar;
    QLabel *statusLabel;
    QTextEdit *quantMatrixDisplay;  // Для отображения матрицы квантования

    void setupUI();
    void displayQuantizationMatrix(const QVector<QVector<int>> &matrix);
};

#endif // MAINWINDOW_H
