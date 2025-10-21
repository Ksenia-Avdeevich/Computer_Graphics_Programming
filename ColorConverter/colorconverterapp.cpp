#include "ColorConverterApp.h"
#include <cmath>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QRegularExpressionValidator>

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

QHBoxLayout* ColorConverterApp::createSliderSpinLayout(QSlider *slider, QWidget *spin, QLineEdit *edit)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(slider);
    layout->addWidget(spin);
    if (edit) {
        layout->addWidget(edit);
    }
    return layout;
}

QHBoxLayout* ColorConverterApp::createRGBRow(QSlider *slider, QSpinBox *spin, QLineEdit *edit)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(slider);
    layout->addWidget(spin);
    layout->addWidget(edit);
    return layout;
}

QHBoxLayout* ColorConverterApp::createCMYKRow(QSlider *slider, QDoubleSpinBox *spin, QLineEdit *edit)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(slider);
    layout->addWidget(spin);
    layout->addWidget(edit);
    return layout;
}

QHBoxLayout* ColorConverterApp::createHSVRow(QSlider *slider, QSpinBox *spin, QLineEdit *edit)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(slider);
    layout->addWidget(spin);
    layout->addWidget(edit);
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

    // Создаем поля для ручного ввода RGB
    rEdit = new QLineEdit;
    gEdit = new QLineEdit;
    bEdit = new QLineEdit;

    // Устанавливаем валидаторы для полей ввода
    QRegularExpressionValidator *rgbValidator = new QRegularExpressionValidator(QRegularExpression("\\d*"), this);
    rEdit->setValidator(rgbValidator);
    gEdit->setValidator(rgbValidator);
    bEdit->setValidator(rgbValidator);

    rEdit->setMaximumWidth(50);
    gEdit->setMaximumWidth(50);
    bEdit->setMaximumWidth(50);

    rEdit->setPlaceholderText("0-255");
    gEdit->setPlaceholderText("0-255");
    bEdit->setPlaceholderText("0-255");

    rgbLayout->addRow("R:", createRGBRow(rSlider, rSpin, rEdit));
    rgbLayout->addRow("G:", createRGBRow(gSlider, gSpin, gEdit));
    rgbLayout->addRow("B:", createRGBRow(bSlider, bSpin, bEdit));

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

    // Создаем поля для ручного ввода CMYK
    cEdit = new QLineEdit;
    mEdit = new QLineEdit;
    yEdit = new QLineEdit;
    kEdit = new QLineEdit;

    // Устанавливаем валидаторы для полей ввода CMYK
    QRegularExpressionValidator *cmykValidator = new QRegularExpressionValidator(QRegularExpression("\\d*\\.?\\d*"), this);
    cEdit->setValidator(cmykValidator);
    mEdit->setValidator(cmykValidator);
    yEdit->setValidator(cmykValidator);
    kEdit->setValidator(cmykValidator);

    cEdit->setMaximumWidth(50);
    mEdit->setMaximumWidth(50);
    yEdit->setMaximumWidth(50);
    kEdit->setMaximumWidth(50);

    cEdit->setPlaceholderText("0-100");
    mEdit->setPlaceholderText("0-100");
    yEdit->setPlaceholderText("0-100");
    kEdit->setPlaceholderText("0-100");

    cmykLayout->addRow("C:", createCMYKRow(cSlider, cSpin, cEdit));
    cmykLayout->addRow("M:", createCMYKRow(mSlider, mSpin, mEdit));
    cmykLayout->addRow("Y:", createCMYKRow(ySlider, ySpin, yEdit));
    cmykLayout->addRow("K:", createCMYKRow(kSlider, kSpin, kEdit));

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

    // Создаем поля для ручного ввода HSV
    hEdit = new QLineEdit;
    sEdit = new QLineEdit;
    vEdit = new QLineEdit;

    // Устанавливаем валидаторы для полей ввода HSV
    QRegularExpressionValidator *hsvValidator = new QRegularExpressionValidator(QRegularExpression("\\d*"), this);
    hEdit->setValidator(hsvValidator);
    sEdit->setValidator(hsvValidator);
    vEdit->setValidator(hsvValidator);

    hEdit->setMaximumWidth(50);
    sEdit->setMaximumWidth(50);
    vEdit->setMaximumWidth(50);

    hEdit->setPlaceholderText("0-359");
    sEdit->setPlaceholderText("0-255");
    vEdit->setPlaceholderText("0-255");

    hsvLayout->addRow("H:", createHSVRow(hSlider, hSpin, hEdit));
    hsvLayout->addRow("S:", createHSVRow(sSlider, sSpin, sEdit));
    hsvLayout->addRow("V:", createHSVRow(vSlider, vSpin, vEdit));

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

    // Подключаем поля ручного ввода RGB
    connect(rEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onRGBTextChanged);
    connect(gEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onRGBTextChanged);
    connect(bEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onRGBTextChanged);

    // CMYK сигналы
    connect(cSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromCMYK);
    connect(mSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromCMYK);
    connect(ySlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromCMYK);
    connect(kSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromCMYK);
    connect(cSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorConverterApp::updateFromCMYK);
    connect(mSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorConverterApp::updateFromCMYK);
    connect(ySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorConverterApp::updateFromCMYK);
    connect(kSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorConverterApp::updateFromCMYK);

    // Подключаем поля ручного ввода CMYK
    connect(cEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onCMYKTextChanged);
    connect(mEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onCMYKTextChanged);
    connect(yEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onCMYKTextChanged);
    connect(kEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onCMYKTextChanged);

    // HSV сигналы
    connect(hSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromHSV);
    connect(sSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromHSV);
    connect(vSlider, &QSlider::valueChanged, this, &ColorConverterApp::updateFromHSV);
    connect(hSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorConverterApp::updateFromHSV);
    connect(sSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorConverterApp::updateFromHSV);
    connect(vSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorConverterApp::updateFromHSV);

    // Подключаем поля ручного ввода HSV
    connect(hEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onHSVTextChanged);
    connect(sEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onHSVTextChanged);
    connect(vEdit, &QLineEdit::editingFinished, this, &ColorConverterApp::onHSVTextChanged);

    // Кнопка выбора цвета
    connect(colorPickerBtn, &QPushButton::clicked, this, &ColorConverterApp::openColorPicker);
}

// Обработка ручного ввода RGB
void ColorConverterApp::onRGBTextChanged()
{
    processRGBInput();
}

// Обработка ручного ввода CMYK
void ColorConverterApp::onCMYKTextChanged()
{
    processCMYKInput();
}

// Обработка ручного ввода HSV
void ColorConverterApp::onHSVTextChanged()
{
    processHSVInput();
}

void ColorConverterApp::processRGBInput()
{
    bool rOk, gOk, bOk;
    int r = rEdit->text().toInt(&rOk);
    int g = gEdit->text().toInt(&gOk);
    int b = bEdit->text().toInt(&bOk);

    if (rOk && gOk && bOk && isValidRGB(r, g, b)) {
        clearWarning();
        rSlider->setValue(r);
        rSpin->setValue(r);
        gSlider->setValue(g);
        gSpin->setValue(g);
        bSlider->setValue(b);
        bSpin->setValue(b);
        updateFromRGB();
    } else {
        showWarning("Ошибка: RGB значения должны быть целыми числами 0-255!");
    }
}

void ColorConverterApp::processCMYKInput()
{
    bool cOk, mOk, yOk, kOk;
    double c = cEdit->text().toDouble(&cOk);
    double m = mEdit->text().toDouble(&mOk);
    double y = yEdit->text().toDouble(&yOk);
    double k = kEdit->text().toDouble(&kOk);

    if (cOk && mOk && yOk && kOk && isValidCMYK(c, m, y, k)) {
        clearWarning();
        cSlider->setValue(static_cast<int>(c * 10));
        cSpin->setValue(c);
        mSlider->setValue(static_cast<int>(m * 10));
        mSpin->setValue(m);
        ySlider->setValue(static_cast<int>(y * 10));
        ySpin->setValue(y);
        kSlider->setValue(static_cast<int>(k * 10));
        kSpin->setValue(k);
        updateFromCMYK();
    } else {
        showWarning("Ошибка: CMYK значения должны быть числами 0.0-100.0!");
    }
}

void ColorConverterApp::processHSVInput()
{
    bool hOk, sOk, vOk;
    int h = hEdit->text().toInt(&hOk);
    int s = sEdit->text().toInt(&sOk);
    int v = vEdit->text().toInt(&vOk);

    if (hOk && sOk && vOk && isValidHSV(h, s, v)) {
        clearWarning();
        hSlider->setValue(h);
        hSpin->setValue(h);
        sSlider->setValue(s);
        sSpin->setValue(s);
        vSlider->setValue(v);
        vSpin->setValue(v);
        updateFromHSV();
    } else {
        showWarning("Ошибка: H должен быть 0-359, S и V должны быть 0-255!");
    }
}

// Проверка валидности RGB значений
bool ColorConverterApp::isValidRGB(int r, int g, int b)
{
    return (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255);
}

// Проверка валидности CMYK значений
bool ColorConverterApp::isValidCMYK(double c, double m, double y, double k)
{
    return (c >= 0.0 && c <= 100.0 && m >= 0.0 && m <= 100.0 &&
            y >= 0.0 && y <= 100.0 && k >= 0.0 && k <= 100.0);
}

// Проверка валидности HSV значений
bool ColorConverterApp::isValidHSV(int h, int s, int v)
{
    return (h >= 0 && h <= 359 && s >= 0 && s <= 255 && v >= 0 && v <= 255);
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

    // Проверка валидности значений
    if (!isValidRGB(r, g, b)) {
        showWarning("Ошибка: Недопустимые значения RGB!");
        updatingFromRGB = false;
        return;
    }

    // Синхронизация слайдеров и спинбоксов
    rSpin->setValue(r);
    gSpin->setValue(g);
    bSpin->setValue(b);

    // Обновление полей ручного ввода
    rEdit->setText(QString::number(r));
    gEdit->setText(QString::number(g));
    bEdit->setText(QString::number(b));

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

    // Обновление полей ручного ввода CMYK
    cEdit->setText(QString::number(c, 'f', 1));
    mEdit->setText(QString::number(m, 'f', 1));
    yEdit->setText(QString::number(y, 'f', 1));
    kEdit->setText(QString::number(k, 'f', 1));

    // Конвертация в HSV
    int h, s, v;
    rgbToHsv(r, g, b, h, s, v);

    hSlider->setValue(h);
    sSlider->setValue(s);
    vSlider->setValue(v);

    hSpin->setValue(h);
    sSpin->setValue(s);
    vSpin->setValue(v);

    // Обновление полей ручного ввода HSV
    hEdit->setText(QString::number(h));
    sEdit->setText(QString::number(s));
    vEdit->setText(QString::number(v));

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

    // Проверка валидности значений
    if (!isValidCMYK(c, m, y, k)) {
        showWarning("Ошибка: Недопустимые значения CMYK!");
        updatingFromCMYK = false;
        return;
    }

    // Синхронизация слайдеров и спинбоксов
    cSpin->setValue(c);
    mSpin->setValue(m);
    ySpin->setValue(y);
    kSpin->setValue(k);

    // Обновление полей ручного ввода
    cEdit->setText(QString::number(c, 'f', 1));
    mEdit->setText(QString::number(m, 'f', 1));
    yEdit->setText(QString::number(y, 'f', 1));
    kEdit->setText(QString::number(k, 'f', 1));

    // Конвертация в RGB
    QColor rgbColor = cmykToRgb(c, m, y, k);

    rSlider->setValue(rgbColor.red());
    gSlider->setValue(rgbColor.green());
    bSlider->setValue(rgbColor.blue());

    rSpin->setValue(rgbColor.red());
    gSpin->setValue(rgbColor.green());
    bSpin->setValue(rgbColor.blue());

    // Обновление полей ручного ввода RGB
    rEdit->setText(QString::number(rgbColor.red()));
    gEdit->setText(QString::number(rgbColor.green()));
    bEdit->setText(QString::number(rgbColor.blue()));

    // Конвертация в HSV
    int h, s, v;
    rgbToHsv(rgbColor.red(), rgbColor.green(), rgbColor.blue(), h, s, v);

    hSlider->setValue(h);
    sSlider->setValue(s);
    vSlider->setValue(v);

    hSpin->setValue(h);
    sSpin->setValue(s);
    vSpin->setValue(v);

    // Обновление полей ручного ввода HSV
    hEdit->setText(QString::number(h));
    sEdit->setText(QString::number(s));
    vEdit->setText(QString::number(v));

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

    // Проверка валидности значений
    if (!isValidHSV(h, s, v)) {
        showWarning("Ошибка: Недопустимые значения HSV!");
        updatingFromHSV = false;
        return;
    }

    // Синхронизация слайдеров и спинбоксов
    hSpin->setValue(h);
    sSpin->setValue(s);
    vSpin->setValue(v);

    // Обновление полей ручного ввода
    hEdit->setText(QString::number(h));
    sEdit->setText(QString::number(s));
    vEdit->setText(QString::number(v));

    // Конвертация в RGB
    QColor rgbColor = hsvToRgb(h, s, v);

    rSlider->setValue(rgbColor.red());
    gSlider->setValue(rgbColor.green());
    bSlider->setValue(rgbColor.blue());

    rSpin->setValue(rgbColor.red());
    gSpin->setValue(rgbColor.green());
    bSpin->setValue(rgbColor.blue());

    // Обновление полей ручного ввода RGB
    rEdit->setText(QString::number(rgbColor.red()));
    gEdit->setText(QString::number(rgbColor.green()));
    bEdit->setText(QString::number(rgbColor.blue()));

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

    // Обновление полей ручного ввода CMYK
    cEdit->setText(QString::number(c, 'f', 1));
    mEdit->setText(QString::number(m, 'f', 1));
    yEdit->setText(QString::number(y, 'f', 1));
    kEdit->setText(QString::number(k, 'f', 1));

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
