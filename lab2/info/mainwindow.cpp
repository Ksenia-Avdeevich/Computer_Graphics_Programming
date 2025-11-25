#include "mainwindow.h"
#include "imageinfo.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QElapsedTimer>
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QFont>
#include <QSplitter>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    showMaximized();
    setWindowTitle("🔍 Image Info Scanner");
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    setWindowTitle("Image Information");

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *controlLayout = new QHBoxLayout();
    QLabel *folderLabel = new QLabel("Папка:", this);
    folderPathEdit = new QLineEdit(this);
    folderPathEdit->setReadOnly(true);

    btnLoadImages = new QPushButton("Загрузить папку", this);
    btnLoadImages->setStyleSheet(R"(
        QPushButton {
            background-color: #34495e;
            color: #ecf0f1;
            padding: 9px 18px;
            border: none;
            border-radius: 5px;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: #2c3e50;
        }
        QPushButton:pressed {
            background-color: #1a252f;
        }
    )");

    controlLayout->addWidget(folderLabel);
    controlLayout->addWidget(folderPathEdit, 1);
    controlLayout->addWidget(btnLoadImages);

    // Создаём сплиттер для таблицы и матрицы квантования
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(9);  // Добавили колонку для степени сжатия
    tableWidget->setHorizontalHeaderLabels(QStringList()
                                           << "Имя файла" << "Размер (пиксели)" << "Разрешение (DPI)"
                                           << "Глубина цвета" << "Сжатие" << "Степень сжатия"
                                           << "Формат" << "Размер файла" << "Доп. информация");

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);

    QFont tableFont("Segoe UI", 11);
    tableWidget->setFont(tableFont);

    tableWidget->setStyleSheet(R"(
        QTableWidget {
            background-color: #2d3748;
            alternate-background-color: #4a5568;
            gridline-color: #4a5568;
            selection-background-color: #3182ce;
            selection-color: white;
            color: #e2e8f0;
            border: 1px solid #4a5568;
            border-radius: 6px;
        }
        QHeaderView::section {
            background-color: #1a202c;
            font-weight: bold;
            padding: 10px;
            border: none;
            border-right: 1px solid #4a5568;
            color: #e2e8f0;
            border-bottom: 2px solid #3182ce;
        }
        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid #4a5568;
        }
    )");

    // Фиксированная ширина колонок
    tableWidget->setColumnWidth(0, 200);  // Имя файла
    tableWidget->setColumnWidth(1, 120);  // Размер (пиксели)
    tableWidget->setColumnWidth(2, 120);  // DPI
    tableWidget->setColumnWidth(3, 100);  // Глубина цвета
    tableWidget->setColumnWidth(4, 100);  // Сжатие
    tableWidget->setColumnWidth(5, 180);  // Степень сжатия
    tableWidget->setColumnWidth(6, 80);   // Формат
    tableWidget->setColumnWidth(7, 100);  // Размер файла
    tableWidget->setColumnWidth(8, 280);  // Доп. информация

    // Панель для отображения матрицы квантования
    QGroupBox *quantBox = new QGroupBox("Матрица квантования JPEG", this);
    QVBoxLayout *quantLayout = new QVBoxLayout(quantBox);

    quantMatrixDisplay = new QTextEdit(this);
    quantMatrixDisplay->setReadOnly(true);
    quantMatrixDisplay->setFont(QFont("Courier New", 10));
    quantMatrixDisplay->setStyleSheet(R"(
        QTextEdit {
            background-color: #1a202c;
            color: #e2e8f0;
            border: 1px solid #4a5568;
            border-radius: 6px;
            padding: 10px;
        }
    )");
    quantMatrixDisplay->setText("Выберите JPEG файл в таблице для просмотра матрицы квантования");

    quantLayout->addWidget(quantMatrixDisplay);
    quantBox->setStyleSheet(R"(
        QGroupBox {
            color: #e2e8f0;
            border: 2px solid #4a5568;
            border-radius: 6px;
            margin-top: 10px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
    )");

    splitter->addWidget(tableWidget);
    splitter->addWidget(quantBox);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    progressBar->setStyleSheet("QProgressBar { height: 25px; }");

    statusLabel = new QLabel("", this);
    statusLabel->setStyleSheet("QLabel { font-style: italic; color: #555; padding: 4px; }");

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(splitter, 1);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(statusLabel);

    setCentralWidget(centralWidget);
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1a202c;
        }
        QWidget {
            background-color: #1a202c;
            color: #e2e8f0;
        }
        QLabel {
            color: #e2e8f0;
        }
        QLineEdit {
            background-color: #2d3748;
            color: #e2e8f0;
            border: 1px solid #4a5568;
            border-radius: 4px;
            padding: 5px;
        }
        QProgressBar {
            background-color: #2d3748;
            border: 1px solid #4a5568;
            border-radius: 4px;
            text-align: center;
            color: #e2e8f0;
        }
        QProgressBar::chunk {
            background-color: #3182ce;
            border-radius: 3px;
        }
    )");

    connect(btnLoadImages, &QPushButton::clicked, this, &MainWindow::onLoadImages);
    connect(tableWidget, &QTableWidget::cellClicked, this, &MainWindow::onTableCellClicked);
}

void MainWindow::onLoadImages()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Выберите папку", QDir::homePath());
    if (folder.isEmpty()) return;

    folderPathEdit->setText(folder);

    QStringList formats = {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif", "*.tif", "*.tiff", "*.pcx"};
    QDirIterator it(folder, formats, QDir::Files, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext() && files.size() < 100000) files.append(it.next());

    if (files.isEmpty()) {
        QMessageBox::information(this, "Информация", "В выбранной папке нет изображений!");
        return;
    }

    tableWidget->setRowCount(0);
    progressBar->setVisible(true);
    progressBar->setRange(0, files.size());
    progressBar->setValue(0);
    btnLoadImages->setEnabled(false);

    QElapsedTimer timer;
    timer.start();

    int processed = 0;
    QVector<ImageInfo> imageInfoList;  // Сохраняем все ImageInfo

    for (const QString &filePath : files) {
        ImageInfo info = getImageInfo(filePath);
        imageInfoList.append(info);

        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        QStringList data = {
            info.fileName, info.size, info.resolution, info.colorDepth,
            info.compression, info.compressionRatio, info.format, info.fileSize, info.additionalInfo
        };

        for (int i = 0; i < data.size(); ++i) {
            QTableWidgetItem* item = new QTableWidgetItem(data[i]);
            item->setTextAlignment(i == 0 || i == 8 ? Qt::AlignLeft : Qt::AlignCenter);

            // Сохраняем путь к файлу и индекс в первой колонке
            if (i == 0) {
                item->setData(Qt::UserRole, filePath);
                item->setData(Qt::UserRole + 1, row);  // Сохраняем индекс для доступа к ImageInfo
            }

            tableWidget->setItem(row, i, item);
        }

        processed++;
        progressBar->setValue(processed);
        if (processed % 10 == 0) QApplication::processEvents();
    }

    // Сохраняем список ImageInfo в свойстве таблицы
    tableWidget->setProperty("imageInfoList", QVariant::fromValue(imageInfoList));

    progressBar->setVisible(false);
    btnLoadImages->setEnabled(true);

    qint64 elapsedMs = timer.elapsed();
    statusLabel->setText(QString("Обработано %1 файлов за %2 мс").arg(files.size()).arg(elapsedMs));
}

void MainWindow::onTableCellClicked(int row, int column)
{
    // Получаем сохраненный список ImageInfo
    QVariant variant = tableWidget->property("imageInfoList");
    if (!variant.isValid()) {
        quantMatrixDisplay->setText("Нет данных об изображениях");
        return;
    }

    QVector<ImageInfo> imageInfoList = variant.value<QVector<ImageInfo>>();

    if (row < 0 || row >= imageInfoList.size()) {
        quantMatrixDisplay->setText("Ошибка: неверный индекс строки");
        return;
    }

    const ImageInfo &info = imageInfoList[row];

    if (info.hasQuantMatrix && !info.quantizationMatrix.isEmpty()) {
        displayQuantizationMatrix(info.quantizationMatrix);
    } else {
        QString formatStr = tableWidget->item(row, 6)->text();
        if (formatStr.toUpper().contains("JPG") || formatStr.toUpper().contains("JPEG")) {
            quantMatrixDisplay->setText("Не удалось извлечь матрицу квантования из этого JPEG файла.");
        } else {
            quantMatrixDisplay->setText("Матрица квантования доступна только для JPEG файлов.\n\nВыбранный формат: " + formatStr);
        }
    }
}

void MainWindow::displayQuantizationMatrix(const QVector<QVector<int>> &matrix)
{
    if (matrix.isEmpty()) {
        quantMatrixDisplay->setText("Матрица квантования пуста");
        return;
    }

    QString text = "Матрица квантования (8x8):\n\n";
    text += "┌─────────────────────────────────────────────────────────────┐\n";

    for (int row = 0; row < matrix.size(); row++) {
        text += "│ ";
        for (int col = 0; col < matrix[row].size(); col++) {
            text += QString("%1").arg(matrix[row][col], 3);
            if (col < matrix[row].size() - 1) text += " ";
        }
        text += " │\n";
    }

    text += "└─────────────────────────────────────────────────────────────┘\n\n";
    text += "Меньшие значения = более высокое качество\n";
    text += "Большие значения = более сильное сжатие";

    quantMatrixDisplay->setText(text);
}
