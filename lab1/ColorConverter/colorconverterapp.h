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
#include <QDoubleSpinBox>

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

    // Новые слоты для ручного ввода
    void onRGBTextChanged();
    void onCMYKTextChanged();
    void onHSVTextChanged();

private:
    // RGB компоненты
    QSlider *rSlider, *gSlider, *bSlider;
    QSpinBox *rSpin, *gSpin, *bSpin;
    QLineEdit *rEdit, *gEdit, *bEdit;

    // CMYK компоненты
    QSlider *cSlider, *mSlider, *ySlider, *kSlider;
    QDoubleSpinBox *cSpin, *mSpin, *ySpin, *kSpin;
    QLineEdit *cEdit, *mEdit, *yEdit, *kEdit;

    // HSV компоненты
    QSlider *hSlider, *sSlider, *vSlider;
    QSpinBox *hSpin, *sSpin, *vSpin;
    QLineEdit *hEdit, *sEdit, *vEdit;

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

    QHBoxLayout* createSliderSpinLayout(QSlider *slider, QWidget *spin, QLineEdit *edit = nullptr);
    QHBoxLayout* createRGBRow(QSlider *slider, QSpinBox *spin, QLineEdit *edit);
    QHBoxLayout* createCMYKRow(QSlider *slider, QDoubleSpinBox *spin, QLineEdit *edit);
    QHBoxLayout* createHSVRow(QSlider *slider, QSpinBox *spin, QLineEdit *edit);

    // Новые методы для проверки значений
    bool isValidRGB(int r, int g, int b);
    bool isValidCMYK(double c, double m, double y, double k);
    bool isValidHSV(int h, int s, int v);

    // Методы для обработки ручного ввода
    void processRGBInput();
    void processCMYKInput();
    void processHSVInput();
};

#endif // COLORCONVERTERAPP_H
