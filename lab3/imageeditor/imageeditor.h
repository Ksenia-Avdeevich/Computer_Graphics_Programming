#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QPixmap>

class ImageEditor : public QMainWindow {
    Q_OBJECT

public:
    ImageEditor(QWidget *parent = nullptr);
    ~ImageEditor();

private slots:
    void openImage();
    void exportImage();
    void enhanceContrast();
    void balanceHistogram();
    void sharpenImage();

private:
    QImage sourceImage;
    QImage modifiedImage;

    QLabel *labelSource;
    QLabel *labelModified;
    QLabel *labelHistSource;
    QLabel *labelHistModified;

    QPushButton *btnOpen;
    QPushButton *btnExport;
    QPushButton *btnContrast;
    QPushButton *btnHistogram;
    QPushButton *btnSharpen;

    QComboBox *comboMethod;

    void createInterface();
    void refreshDisplay();
    QPixmap createHistogram(const QImage &img);
};

#endif // IMAGEEDITOR_H
