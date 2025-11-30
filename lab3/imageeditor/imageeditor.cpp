#include "imageeditor.h"
#include "imageoperations.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QPixmap>
#include <QFrame>
#include <algorithm>

ImageEditor::ImageEditor(QWidget *parent)
    : QMainWindow(parent)
{
    createInterface();
}

ImageEditor::~ImageEditor() {}

void ImageEditor::createInterface()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    labelSource = new QLabel("Исходное изображение");
    labelModified = new QLabel("Обработанное изображение");
    labelHistSource = new QLabel("Гистограмма исходного");
    labelHistModified = new QLabel("Гистограмма обработанного");

    QList<QLabel*> labels = {labelSource, labelModified, labelHistSource, labelHistModified};
    for (auto lbl : labels) {
        lbl->setFrameStyle(QFrame::Box | QFrame::Plain);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        lbl->setStyleSheet("background-color: #f0f0f0; border-radius: 8px;");
    }

    btnOpen = new QPushButton("Открыть изображение");
    btnExport = new QPushButton("Сохранить результат");
    btnContrast = new QPushButton("Усилить контраст");
    btnHistogram = new QPushButton("Балансировка гистограммы");
    btnSharpen = new QPushButton("Увеличить резкость");

    comboMethod = new QComboBox();
    comboMethod->addItem("RGB (по каналам)");
    comboMethod->addItem("HSV (по яркости)");

    QList<QWidget*> controls = {btnOpen, btnExport, btnContrast, btnHistogram, comboMethod, btnSharpen};
    for (auto w : controls) {
        w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        w->setStyleSheet(
            "QPushButton, QComboBox {"
            "background-color: #0078d7;"
            "color: white;"
            "border-radius: 6px;"
            "padding: 6px;"
            "font: 14px 'Segoe UI';"
            "}"
            "QPushButton:hover, QComboBox:hover {"
            "background-color: #005a9e;"
            "}"
            );
    }

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(labelSource, 0, 0);
    gridLayout->addWidget(labelModified, 0, 1);
    gridLayout->addWidget(labelHistSource, 1, 0);
    gridLayout->addWidget(labelHistModified, 1, 1);
    gridLayout->setRowStretch(0, 4);
    gridLayout->setRowStretch(1, 2);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setSpacing(15);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnOpen);
    buttonLayout->addWidget(btnExport);
    buttonLayout->addWidget(btnContrast);
    buttonLayout->addWidget(btnHistogram);
    buttonLayout->addWidget(comboMethod);
    buttonLayout->addWidget(btnSharpen);
    buttonLayout->setSpacing(10);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(gridLayout);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    central->setLayout(mainLayout);

    connect(btnOpen, &QPushButton::clicked, this, &ImageEditor::openImage);
    connect(btnExport, &QPushButton::clicked, this, &ImageEditor::exportImage);
    connect(btnContrast, &QPushButton::clicked, this, &ImageEditor::enhanceContrast);
    connect(btnHistogram, &QPushButton::clicked, this, &ImageEditor::balanceHistogram);
    connect(btnSharpen, &QPushButton::clicked, this, &ImageEditor::sharpenImage);
}

void ImageEditor::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выбрать изображение", "", "Images (*.png *.jpg *.bmp *.jpeg)");
    if (!fileName.isEmpty()) {
        if (!sourceImage.load(fileName)) return;
        sourceImage = sourceImage.convertToFormat(QImage::Format_RGB32);
        modifiedImage = sourceImage;
        refreshDisplay();
    }
}

void ImageEditor::exportImage()
{
    if (modifiedImage.isNull()) return;
    QString fileName = QFileDialog::getSaveFileName(this, "Экспорт изображения", "", "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)");
    if (!fileName.isEmpty()) modifiedImage.save(fileName);
}

void ImageEditor::enhanceContrast() {
    if (sourceImage.isNull()) return;
    modifiedImage = ImageOperations::adjustContrast(sourceImage);
    refreshDisplay();
}

void ImageEditor::balanceHistogram() {
    if (sourceImage.isNull()) return;
    QString method = comboMethod->currentText();
    if (method.startsWith("RGB"))
        modifiedImage = ImageOperations::balanceRGB(sourceImage);
    else
        modifiedImage = ImageOperations::balanceHSV(sourceImage);
    refreshDisplay();
}

void ImageEditor::sharpenImage() {
    if (sourceImage.isNull()) return;
    modifiedImage = ImageOperations::enhanceSharpness(sourceImage);
    refreshDisplay();
}

void ImageEditor::refreshDisplay()
{
    if (!sourceImage.isNull()) {
        labelSource->setPixmap(QPixmap::fromImage(sourceImage).scaled(labelSource->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        labelHistSource->setPixmap(createHistogram(sourceImage));
    } else {
        labelSource->clear();
        labelHistSource->clear();
    }

    if (!modifiedImage.isNull()) {
        labelModified->setPixmap(QPixmap::fromImage(modifiedImage).scaled(labelModified->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        labelHistModified->setPixmap(createHistogram(modifiedImage));
    } else {
        labelModified->clear();
        labelHistModified->clear();
    }
}

QPixmap ImageEditor::createHistogram(const QImage &img)
{
    if (img.isNull()) return QPixmap();

    QVector<int> hist(256, 0);
    for (int y = 0; y < img.height(); ++y) {
        const QRgb *line = reinterpret_cast<const QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            int gray = qGray(line[x]);
            hist[gray]++;
        }
    }

    int maxVal = *std::max_element(hist.begin(), hist.end());
    if (maxVal == 0) maxVal = 1;

    QPixmap pix(256, 120);
    pix.fill(Qt::white);
    QPainter p(&pix);
    p.setPen(Qt::black);
    for (int i = 0; i < 256; ++i) {
        int h = hist[i] * 100 / maxVal;
        p.drawLine(i, 110, i, 110 - h);
    }
    p.drawRect(0, 0, pix.width()-1, pix.height()-1);
    return pix;
}
