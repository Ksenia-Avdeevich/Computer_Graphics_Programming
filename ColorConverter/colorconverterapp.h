#ifndef COLORCONVERTERAPP_H
#define COLORCONVERTERAPP_H

#include <QMainWindow>
#include <QWidget>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QColorDialog>
#include <QMessageBox>
#include <QHBoxLayout>

class ColorConverterApp : public QMainWindow
{
    Q_OBJECT

public:
    ColorConverterApp(QWidget *parent = nullptr);
    ~ColorConverterApp();

private slots:
    void updateFromRGB();
    void updateFromCMYK();
    void updateFromHSV();
    void openColorPicker();
    void updateColorDisplay();

private:
    // RGB компоненты
    QSlider *rSlider, *gSlider, *bSlider;
    QSpinBox *rSpin, *gSpin, *bSpin;

    // CMYK компоненты
    QSlider *cSlider, *mSlider, *ySlider, *kSlider;
    QDoubleSpinBox *cSpin, *mSpin, *ySpin, *kSpin;

    // HSV компоненты
    QSlider *hSlider, *sSlider, *vSlider;
    QSpinBox *hSpin, *sSpin, *vSpin;

    // Элементы интерфейса
    QLabel *colorDisplay;
    QPushButton *colorPickerBtn;
    QLabel *warningLabel;

    // Флаги для предотвращения рекурсивных обновлений
    bool updatingFromRGB;
    bool updatingFromCMYK;
    bool updatingFromHSV;

    // Текущий цвет
    QColor currentColor;

    // Методы конвертации
    QColor cmykToRgb(double c, double m, double y, double k);
    void rgbToCmyk(int r, int g, int b, double &c, double &m, double &y, double &k);
    QColor hsvToRgb(int h, int s, int v);
    void rgbToHsv(int r, int g, int b, int &h, int &s, int &v);

    // Вспомогательные методы
    void setupUI();
    void connectSignals();
    void showWarning(const QString &message);
    void clearWarning();

    // Добавлен объявление недостающего метода
    QHBoxLayout* createSliderSpinLayout(QSlider *slider, QWidget *spin);
};

#endif // COLORCONVERTERAPP_H
