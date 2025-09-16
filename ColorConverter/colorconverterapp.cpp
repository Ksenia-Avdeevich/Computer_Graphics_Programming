#include "ColorConverterApp.h"
#include <cmath>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>

ColorConverterApp::ColorConverterApp(QWidget *parent)
    : QMainWindow(parent), updatingFromRGB(false), updatingFromCMYK(false), updatingFromHSV(false)
{
    setWindowTitle("Конвертер цветовых моделей - CMYK ↔ RGB ↔ HSV");
    setMinimumSize(800, 600);

    setupUI();
    connectSignals();

    // Установка начального цвета
    currentColor = QColor(255, 0, 0); // Красный
    updateFromRGB();
}

ColorConverterApp::~ColorConverterApp() {}

// Добавлено определение метода createSliderSpinLayout
QHBoxLayout* ColorConverterApp::createSliderSpinLayout(QSlider *slider, QWidget *spin)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(slider);
    layout->addWidget(spin);
    return layout;
}

void ColorConverterApp::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Отображение цвета
    colorDisplay = new QLabel(this);
    colorDisplay->setMinimumHeight(100);
    colorDisplay->setFrameStyle(QFrame::Box);
    colorDisplay->setAlignment(Qt::AlignCenter);

    // Кнопка выбора цвета
    colorPickerBtn = new QPushButton("Выбрать цвет из палитры", this);

    // Предупреждение
    warningLabel = new QLabel(this);
    warningLabel->setStyleSheet("color: red;");
    warningLabel->setAlignment(Qt::AlignCenter);
    warningLabel->hide();

    // Группа RGB
    QGroupBox *rgbGroup = new QGroupBox("RGB Model", this);
    QFormLayout *rgbLayout = new QFormLayout(rgbGroup);

    rSlider = new QSlider(Qt::Horizontal);
    gSlider = new QSlider(Qt::Horizontal);
    bSlider = new QSlider(Qt::Horizontal);

    rSlider->setRange(0, 255);
    gSlider->setRange(0, 255);
    bSlider->setRange(0, 255);

    rSpin = new QSpinBox;
    gSpin = new QSpinBox;
    bSpin = new QSpinBox;

    rSpin->setRange(0, 255);
    gSpin->setRange(0, 255);
    bSpin->setRange(0, 255);

    rgbLayout->addRow("R:", createSliderSpinLayout(rSlider, rSpin));
    rgbLayout->addRow("G:", createSliderSpinLayout(gSlider, gSpin));
    rgbLayout->addRow("B:", createSliderSpinLayout(bSlider, bSpin));

    // Группа CMYK
    QGroupBox *cmykGroup = new QGroupBox("CMYK Model", this);
    QFormLayout *cmykLayout = new QFormLayout(cmykGroup);

    cSlider = new QSlider(Qt::Horizontal);
    mSlider = new QSlider(Qt::Horizontal);
    ySlider = new QSlider(Qt::Horizontal);
    kSlider = new QSlider(Qt::Horizontal);

    cSlider->setRange(0, 1000);
    mSlider->setRange(0, 1000);
    ySlider->setRange(0, 1000);
    kSlider->setRange(0, 1000);

    cSpin = new QDoubleSpinBox;
    mSpin = new QDoubleSpinBox;
    ySpin = new QDoubleSpinBox;
    kSpin = new QDoubleSpinBox;

    cSpin->setRange(0.0, 100.0);
    mSpin->setRange(0.0, 100.0);
    ySpin->setRange(0.0, 100.0);
    kSpin->setRange(0.0, 100.0);
    cSpin->setDecimals(1);
    mSpin->setDecimals(1);
    ySpin->setDecimals(1);
    kSpin->setDecimals(1);

    cmykLayout->addRow("C:", createSliderSpinLayout(cSlider, cSpin));
    cmykLayout->addRow("M:", createSliderSpinLayout(mSlider, mSpin));
    cmykLayout->addRow("Y:", createSliderSpinLayout(ySlider, ySpin));
    cmykLayout->addRow("K:", createSliderSpinLayout(kSlider, kSpin));

    // Группа HSV
    QGroupBox *hsvGroup = new QGroupBox("HSV Model", this);
    QFormLayout *hsvLayout = new QFormLayout(hsvGroup);

    hSlider = new QSlider(Qt::Horizontal);
    sSlider = new QSlider(Qt::Horizontal);
    vSlider = new QSlider(Qt::Horizontal);

    hSlider->setRange(0, 359);
    sSlider->setRange(0, 255);
    vSlider->setRange(0, 255);

    hSpin = new QSpinBox;
    sSpin = new QSpinBox;
    vSpin = new QSpinBox;

    hSpin->setRange(0, 359);
    sSpin->setRange(0, 255);
    vSpin->setRange(0, 255);

    hsvLayout->addRow("H:", createSliderSpinLayout(hSlider, hSpin));
    hsvLayout->addRow("S:", createSliderSpinLayout(sSlider, sSpin));
    hsvLayout->addRow("V:", createSliderSpinLayout(vSlider, vSpin));

    // Компоновка
    mainLayout->addWidget(colorDisplay);
    mainLayout->addWidget(colorPickerBtn);
    mainLayout->addWidget(warningLabel);
    mainLayout->addWidget(rgbGroup);
    mainLayout->addWidget(cmykGroup);
    mainLayout->addWidget(hsvGroup);

    setCentralWidget(centralWidget);
}

void ColorConverterApp::connectSignals()
{
    // RGB сигналы
    connect(rSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromRGB);
    connect(gSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromRGB);
    connect(bSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromRGB);
    connect(rSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorConverterApp::updateFromRGB);
    connect(gSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorConverterApp::updateFromRGB);
    connect(bSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorConverterApp::updateFromRGB);

    // CMYK сигналы
    connect(cSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromCMYK);
    connect(mSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromCMYK);
    connect(ySlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromCMYK);
    connect(kSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromCMYK);
    connect(cSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorConverterApp::updateFromCMYK);
    connect(mSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorConverterApp::updateFromCMYK);
    connect(ySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorConverterApp::updateFromCMYK);
    connect(kSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorConverterApp::updateFromCMYK);

    // HSV сигналы
    connect(hSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromHSV);
    connect(sSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromHSV);
    connect(vSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromHSV);
    connect(hSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorConverterApp::updateFromHSV);
    connect(sSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorConverterApp::updateFromHSV);
    connect(vSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorConverterApp::updateFromHSV);

    // Кнопка выбора цвета
    connect(colorPickerBtn, &QPushButton::clicked, this, &ColorConverterApp::openColorPicker);
}

// Конвертация CMYK to RGB
QColor ColorConverterApp::cmykToRgb(double c, double m, double y, double k)
{
    c = qBound(0.0, c, 100.0) / 100.0;
    m = qBound(0.0, m, 100.0) / 100.0;
    y = qBound(0.0, y, 100.0) / 100.0;
    k = qBound(0.0, k, 100.0) / 100.0;

    int r = static_cast<int>(255 * (1 - c) * (1 - k));
    int g = static_cast<int>(255 * (1 - m) * (1 - k));
    int b = static_cast<int>(255 * (1 - y) * (1 - k));

    return QColor(qBound(0, r, 255), qBound(0, g, 255), qBound(0, b, 255));
}

// Конвертация RGB to CMYK
void ColorConverterApp::rgbToCmyk(int r, int g, int b, double &c, double &m, double &y, double &k)
{
    r = qBound(0, r, 255);
    g = qBound(0, g, 255);
    b = qBound(0, b, 255);

    double dr = r / 255.0;
    double dg = g / 255.0;
    double db = b / 255.0;

    k = 1 - qMax(dr, qMax(dg, db));

    if (k == 1.0) {
        c = m = y = 0;
    } else {
        c = (1 - dr - k) / (1 - k) * 100;
        m = (1 - dg - k) / (1 - k) * 100;
        y = (1 - db - k) / (1 - k) * 100;
        k *= 100;
    }

    c = qBound(0.0, c, 100.0);
    m = qBound(0.0, m, 100.0);
    y = qBound(0.0, y, 100.0);
    k = qBound(0.0, k, 100.0);
}

// Конвертация HSV to RGB
QColor ColorConverterApp::hsvToRgb(int h, int s, int v)
{
    h = qBound(0, h, 359);
    s = qBound(0, s, 255);
    v = qBound(0, v, 255);

    double hue = h / 60.0;
    double saturation = s / 255.0;
    double value = v / 255.0;

    int hi = static_cast<int>(hue) % 6;
    double f = hue - hi;

    double p = value * (1 - saturation);
    double q = value * (1 - f * saturation);
    double t = value * (1 - (1 - f) * saturation);

    double r, g, b;

    switch (hi) {
    case 0: r = value; g = t; b = p; break;
    case 1: r = q; g = value; b = p; break;
    case 2: r = p; g = value; b = t; break;
    case 3: r = p; g = q; b = value; break;
    case 4: r = t; g = p; b = value; break;
    case 5: r = value; g = p; b = q; break;
    default: r = g = b = 0;
    }

    return QColor(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
}

// Конвертация RGB to HSV
void ColorConverterApp::rgbToHsv(int r, int g, int b, int &h, int &s, int &v)
{
    r = qBound(0, r, 255);
    g = qBound(0, g, 255);
    b = qBound(0, b, 255);

    double dr = r / 255.0;
    double dg = g / 255.0;
    double db = b / 255.0;

    double cmax = qMax(dr, qMax(dg, db));
    double cmin = qMin(dr, qMin(dg, db));
    double delta = cmax - cmin;

    // Hue calculation
    if (delta == 0) {
        h = 0;
    } else if (cmax == dr) {
        h = static_cast<int>(60 * fmod(((dg - db) / delta), 6));
    } else if (cmax == dg) {
        h = static_cast<int>(60 * (((db - dr) / delta) + 2));
    } else {
        h = static_cast<int>(60 * (((dr - dg) / delta) + 4));
    }

    if (h < 0) h += 360;
    h = qBound(0, h, 359);

    // Saturation calculation
    s = cmax == 0 ? 0 : static_cast<int>((delta / cmax) * 255);
    s = qBound(0, s, 255);

    // Value calculation
    v = static_cast<int>(cmax * 255);
    v = qBound(0, v, 255);
}

void ColorConverterApp::updateFromRGB()
{
    if (updatingFromCMYK || updatingFromHSV) return;

    updatingFromRGB = true;
    clearWarning();

    int r = rSlider->value();
    int g = gSlider->value();
    int b = bSlider->value();

    // Синхронизация слайдеров и спинбоксов
    rSpin->setValue(r);
    gSpin->setValue(g);
    bSpin->setValue(b);

    // Конвертация в CMYK
    double c, m, y, k;
    rgbToCmyk(r, g, b, c, m, y, k);

    cSlider->setValue(static_cast<int>(c * 10));
    mSlider->setValue(static_cast<int>(m * 10));
    ySlider->setValue(static_cast<int>(y * 10));
    kSlider->setValue(static_cast<int>(k * 10));

    cSpin->setValue(c);
    mSpin->setValue(m);
    ySpin->setValue(y);
    kSpin->setValue(k);

    // Конвертация в HSV
    int h, s, v;
    rgbToHsv(r, g, b, h, s, v);

    hSlider->setValue(h);
    sSlider->setValue(s);
    vSlider->setValue(v);

    hSpin->setValue(h);
    sSpin->setValue(s);
    vSpin->setValue(v);

    // Обновление цвета
    currentColor = QColor(r, g, b);
    updateColorDisplay();

    updatingFromRGB = false;
}

void ColorConverterApp::updateFromCMYK()
{
    if (updatingFromRGB || updatingFromHSV) return;

    updatingFromCMYK = true;
    clearWarning();

    double c = cSlider->value() / 10.0;
    double m = mSlider->value() / 10.0;
    double y = ySlider->value() / 10.0;
    double k = kSlider->value() / 10.0;

    // Синхронизация слайдеров и спинбоксов
    cSpin->setValue(c);
    mSpin->setValue(m);
    ySpin->setValue(y);
    kSpin->setValue(k);

    // Конвертация в RGB
    QColor rgbColor = cmykToRgb(c, m, y, k);

    rSlider->setValue(rgbColor.red());
    gSlider->setValue(rgbColor.green());
    bSlider->setValue(rgbColor.blue());

    rSpin->setValue(rgbColor.red());
    gSpin->setValue(rgbColor.green());
    bSpin->setValue(rgbColor.blue());

    // Конвертация в HSV
    int h, s, v;
    rgbToHsv(rgbColor.red(), rgbColor.green(), rgbColor.blue(), h, s, v);

    hSlider->setValue(h);
    sSlider->setValue(s);
    vSlider->setValue(v);

    hSpin->setValue(h);
    sSpin->setValue(s);
    vSpin->setValue(v);

    // Обновление цвета
    currentColor = rgbColor;
    updateColorDisplay();

    updatingFromCMYK = false;
}

void ColorConverterApp::updateFromHSV()
{
    if (updatingFromRGB || updatingFromCMYK) return;

    updatingFromHSV = true;
    clearWarning();

    int h = hSlider->value();
    int s = sSlider->value();
    int v = vSlider->value();

    // Синхронизация слайдеров и спинбоксов
    hSpin->setValue(h);
    sSpin->setValue(s);
    vSpin->setValue(v);

    // Конвертация в RGB
    QColor rgbColor = hsvToRgb(h, s, v);

    rSlider->setValue(rgbColor.red());
    gSlider->setValue(rgbColor.green());
    bSlider->setValue(rgbColor.blue());

    rSpin->setValue(rgbColor.red());
    gSpin->setValue(rgbColor.green());
    bSpin->setValue(rgbColor.blue());

    // Конвертация в CMYK
    double c, m, y, k;
    rgbToCmyk(rgbColor.red(), rgbColor.green(), rgbColor.blue(), c, m, y, k);

    cSlider->setValue(static_cast<int>(c * 10));
    mSlider->setValue(static_cast<int>(m * 10));
    ySlider->setValue(static_cast<int>(y * 10));
    kSlider->setValue(static_cast<int>(k * 10));

    cSpin->setValue(c);
    mSpin->setValue(m);
    ySpin->setValue(y);
    kSpin->setValue(k);

    // Обновление цвета
    currentColor = rgbColor;
    updateColorDisplay();

    updatingFromHSV = false;
}

void ColorConverterApp::openColorPicker()
{
    QColor color = QColorDialog::getColor(currentColor, this, "Выберите цвет");
    if (color.isValid()) {
        currentColor = color;

        // Установка RGB значений
        rSlider->setValue(color.red());
        gSlider->setValue(color.green());
        bSlider->setValue(color.blue());

        updateFromRGB();
    }
}

void ColorConverterApp::updateColorDisplay()
{
    QString style = QString("background-color: %1; color: %2;")
    .arg(currentColor.name())
        .arg(currentColor.lightness() > 128 ? "black" : "white");

    QString text = QString("Текущий цвет: %1\nRGB: %2, %3, %4")
                       .arg(currentColor.name())
                       .arg(currentColor.red())
                       .arg(currentColor.green())
                       .arg(currentColor.blue());

    colorDisplay->setStyleSheet(style);
    colorDisplay->setText(text);
}

void ColorConverterApp::showWarning(const QString &message)
{
    warningLabel->setText(message);
    warningLabel->show();
}

void ColorConverterApp::clearWarning()
{
    warningLabel->clear();
    warningLabel->hide();
}
