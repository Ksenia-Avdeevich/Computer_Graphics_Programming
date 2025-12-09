#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QPointF>
#include <cmath>

// Структуры данных
struct Line {
    QPointF p1, p2;
    Line() = default;
    Line(double x1, double y1, double x2, double y2) : p1(x1, y1), p2(x2, y2) {}
};

struct ClipWindow {
    double xmin, ymin, xmax, ymax;
    ClipWindow() : xmin(0), ymin(0), xmax(0), ymax(0) {}
    ClipWindow(double xmin, double ymin, double xmax, double ymax)
        : xmin(xmin), ymin(ymin), xmax(xmax), ymax(ymax) {}
};

// Класс с алгоритмами отсечения
class ClippingAlgorithms {
public:
    explicit ClippingAlgorithms(const ClipWindow &w) : window(w) {}

    // Алгоритм Сазерленда-Коэна
    bool cohenSutherland(const Line &input, Line &output) {
        double x1 = input.p1.x(), y1 = input.p1.y();
        double x2 = input.p2.x(), y2 = input.p2.y();
        int code1 = computeCode(x1, y1);
        int code2 = computeCode(x2, y2);
        bool accept = false;

        while (true) {
            if (!(code1 | code2)) {
                accept = true;
                break;
            } else if (code1 & code2) {
                break;
            } else {
                double x, y;
                int codeOut = code1 ? code1 : code2;

                if (codeOut & TOP) {
                    x = x1 + (x2 - x1) * (window.ymax - y1) / (y2 - y1);
                    y = window.ymax;
                } else if (codeOut & BOTTOM) {
                    x = x1 + (x2 - x1) * (window.ymin - y1) / (y2 - y1);
                    y = window.ymin;
                } else if (codeOut & RIGHT) {
                    y = y1 + (y2 - y1) * (window.xmax - x1) / (x2 - x1);
                    x = window.xmax;
                } else {
                    y = y1 + (y2 - y1) * (window.xmin - x1) / (x2 - x1);
                    x = window.xmin;
                }

                if (codeOut == code1) {
                    x1 = x; y1 = y;
                    code1 = computeCode(x1, y1);
                } else {
                    x2 = x; y2 = y;
                    code2 = computeCode(x2, y2);
                }
            }
        }

        if (accept) output = Line(x1, y1, x2, y2);
        return accept;
    }

    // Алгоритм Лианга-Барски
    bool liangBarsky(const Line &input, Line &output) {
        double x1 = input.p1.x(), y1 = input.p1.y();
        double x2 = input.p2.x(), y2 = input.p2.y();
        double dx = x2 - x1, dy = y2 - y1;
        double p[4] = {-dx, dx, -dy, dy};
        double q[4] = {x1 - window.xmin, window.xmax - x1,
                       y1 - window.ymin, window.ymax - y1};
        double u1 = 0.0, u2 = 1.0;

        for (int i = 0; i < 4; ++i) {
            if (p[i] == 0.0) {
                if (q[i] < 0.0) return false;
            } else {
                double t = q[i] / p[i];
                if (p[i] < 0.0) {
                    if (t > u2) return false;
                    if (t > u1) u1 = t;
                } else {
                    if (t < u1) return false;
                    if (t < u2) u2 = t;
                }
            }
        }

        output = Line(x1 + u1 * dx, y1 + u1 * dy, x1 + u2 * dx, y1 + u2 * dy);
        return true;
    }

    // Алгоритм средней точки
    bool midpointClip(const Line &input, Line &output) {
        double x1 = input.p1.x(), y1 = input.p1.y();
        double x2 = input.p2.x(), y2 = input.p2.y();

        if (isInside(x1, y1) && isInside(x2, y2)) {
            output = input;
            return true;
        }

        int code1 = computeCode(x1, y1);
        int code2 = computeCode(x2, y2);
        if (code1 & code2) return false;

        int iterations = 0;
        while (iterations++ < 20) {
            double mx = (x1 + x2) / 2.0;
            double my = (y1 + y2) / 2.0;
            int mc = computeCode(mx, my);

            if (std::abs(x2 - x1) < 0.5 && std::abs(y2 - y1) < 0.5) {
                if (isInside(mx, my)) {
                    output = Line(mx, my, mx, my);
                    return true;
                }
                return false;
            }

            if (code1 == INSIDE) {
                if (mc == INSIDE) { x2 = mx; y2 = my; code2 = mc; }
                else { x1 = mx; y1 = my; code1 = mc; }
            } else if (code2 == INSIDE) {
                if (mc == INSIDE) { x1 = mx; y1 = my; code1 = mc; }
                else { x2 = mx; y2 = my; code2 = mc; }
            } else {
                if (code1 & code2) return false;
                if (mc == INSIDE) {
                    if (std::abs(x1 - mx) > std::abs(x2 - mx)) {
                        x1 = mx; y1 = my; code1 = mc;
                    } else {
                        x2 = mx; y2 = my; code2 = mc;
                    }
                } else {
                    if ((code1 & mc) != 0) { x1 = mx; y1 = my; code1 = mc; }
                    else { x2 = mx; y2 = my; code2 = mc; }
                }
            }
        }
        return cohenSutherland(Line(x1, y1, x2, y2), output);
    }

    // Отсечение выпуклым многоугольником
    bool convexPolygonClip(const Line &input, Line &output) {
        return liangBarsky(input, output);
    }

private:
    ClipWindow window;
    enum { INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8 };

    int computeCode(double x, double y) const {
        int code = INSIDE;
        if (x < window.xmin) code |= LEFT;
        else if (x > window.xmax) code |= RIGHT;
        if (y < window.ymin) code |= BOTTOM;
        else if (y > window.ymax) code |= TOP;
        return code;
    }

    bool isInside(double x, double y) const {
        return x >= window.xmin && x <= window.xmax &&
               y >= window.ymin && y <= window.ymax;
    }
};

// Виджет отрисовки
class RenderWidget : public QWidget {
public:
    RenderWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumSize(800, 600);
        scale = 1.0;
        offsetX = 50.0;
        offsetY = 50.0;
    }

    void setData(const QVector<Line> &l, const ClipWindow &w, int alg, bool show) {
        lines = l;
        window = w;
        algorithm = alg;
        showOriginal = show;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // Определим масштаб и смещение для отображения
        double dataWidth = std::max(window.xmax, 500.0) - std::min(window.xmin, 0.0);
        double dataHeight = std::max(window.ymax, 500.0) - std::min(window.ymin, 0.0);

        scale = std::min((width() - 100) / dataWidth, (height() - 100) / dataHeight);
        offsetX = -std::min(window.xmin, 0.0) * scale + 50;
        offsetY = height() - (-std::min(window.ymin, 0.0) * scale + 50);

        // Сетка с шагом 50 в координатах данных
        p.setPen(QPen(QColor(224, 224, 224), 1));
        double gridStep = 50.0;
        double startGridX = std::floor(window.xmin / gridStep) * gridStep;
        double endGridX = std::ceil(window.xmax / gridStep) * gridStep;
        double startGridY = std::floor(window.ymin / gridStep) * gridStep;
        double endGridY = std::ceil(window.ymax / gridStep) * gridStep;

        for (double x = startGridX; x <= endGridX; x += gridStep) {
            int screenX = transform(QPointF(x, 0)).x();
            p.drawLine(screenX, 0, screenX, height());
        }
        for (double y = startGridY; y <= endGridY; y += gridStep) {
            int screenY = transform(QPointF(0, y)).y();
            p.drawLine(0, screenY, width(), screenY);
        }

        // Оси координат
        p.setPen(QPen(QColor(102, 102, 102), 2));
        // Ось X
        QPointF axisXStart = transform(QPointF(std::min(window.xmin, 0.0), 0));
        QPointF axisXEnd = transform(QPointF(std::max(window.xmax, dataWidth), 0));
        p.drawLine(axisXStart, axisXEnd);
        // Ось Y
        QPointF axisYStart = transform(QPointF(0, std::min(window.ymin, 0.0)));
        QPointF axisYEnd = transform(QPointF(0, std::max(window.ymax, dataHeight)));
        p.drawLine(axisYStart, axisYEnd);

        // Стрелки осей
        drawArrow(p, axisXEnd, 0); // стрелка оси X
        drawArrow(p, axisYEnd, 90); // стрелка оси Y

        // Подписи осей
        p.drawText(axisXEnd + QPointF(10, 10), "X");
        p.drawText(axisYEnd + QPointF(-10, -10), "Y");

        // Засечки и подписи на осях
        for (double x = startGridX; x <= endGridX; x += gridStep) {
            if (x == 0) continue;
            QPointF tickPos = transform(QPointF(x, 0));
            p.drawLine(tickPos.x(), tickPos.y() - 5, tickPos.x(), tickPos.y() + 5);
            p.drawText(tickPos + QPointF(-10, 20), QString::number(x));
        }
        for (double y = startGridY; y <= endGridY; y += gridStep) {
            if (y == 0) continue;
            QPointF tickPos = transform(QPointF(0, y));
            p.drawLine(tickPos.x() - 5, tickPos.y(), tickPos.x() + 5, tickPos.y());
            p.drawText(tickPos + QPointF(-30, 5), QString::number(y));
        }

        // Окно отсечения
        QPointF p1 = transform(QPointF(window.xmin, window.ymin));
        QPointF p2 = transform(QPointF(window.xmax, window.ymax));
        p.fillRect(QRectF(p1.x(), p1.y(), p2.x() - p1.x(), p2.y() - p1.y()),
                   QColor(37, 99, 235, 25));
        p.setPen(QPen(QColor(37, 99, 235), 2));
        p.drawRect(QRectF(p1.x(), p1.y(), p2.x() - p1.x(), p2.y() - p1.y()));

        // Исходные отрезки
        if (showOriginal) {
            p.setPen(QPen(QColor(156, 163, 175), 1, Qt::DashLine));
            for (const Line &line : lines) {
                p.drawLine(transform(line.p1), transform(line.p2));
            }
        }

        // Отсеченные отрезки
        p.setPen(QPen(QColor(220, 38, 38), 3));
        ClippingAlgorithms clipper(window);
        for (const Line &line : lines) {
            Line clipped;
            bool visible = false;
            switch (algorithm) {
            case 0: visible = clipper.cohenSutherland(line, clipped); break;
            case 1: visible = clipper.liangBarsky(line, clipped); break;
            case 2: visible = clipper.midpointClip(line, clipped); break;
            case 3: visible = clipper.convexPolygonClip(line, clipped); break;
            }
            if (visible) {
                p.drawLine(transform(clipped.p1), transform(clipped.p2));
            }
        }
    }

private:
    QVector<Line> lines;
    ClipWindow window;
    int algorithm = 0;
    bool showOriginal = true;
    double scale, offsetX, offsetY;

    QPointF transform(const QPointF &p) const {
        return QPointF(p.x() * scale + offsetX, -p.y() * scale + offsetY);
    }

    void drawArrow(QPainter &p, const QPointF &pos, double angle) {
        p.save();
        p.translate(pos);
        p.rotate(angle);
        QPointF arrow[3] = {QPointF(0, 0), QPointF(-10, -5), QPointF(-10, 5)};
        p.drawPolygon(arrow, 3);
        p.restore();
    }
};

// Главное окно
class MainWindow : public QMainWindow {
public:
    MainWindow() {
        setWindowTitle("Pruning Algorithms");
        resize(1200, 700);

        QWidget *central = new QWidget;
        QHBoxLayout *mainLayout = new QHBoxLayout(central);

        // Левая панель
        QWidget *leftPanel = new QWidget;
        leftPanel->setMaximumWidth(250);
        QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

        // Алгоритм
        leftLayout->addWidget(new QLabel("Алгоритм:"));
        algoCombo = new QComboBox;
        algoCombo->addItem("Сазерленда-Коэна");
        algoCombo->addItem("Лианга-Барски");
        algoCombo->addItem("Средней точки");
        connect(algoCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::updateView);
        leftLayout->addWidget(algoCombo);

        // Управление
        showOriginalCheck = new QCheckBox("Показать исходные отрезки");
        showOriginalCheck->setChecked(true);
        connect(showOriginalCheck, &QCheckBox::stateChanged, this, &MainWindow::updateView);
        leftLayout->addWidget(showOriginalCheck);

        // Кнопки
        QPushButton *sampleBtn = new QPushButton("Пример данных");
        connect(sampleBtn, &QPushButton::clicked, this, &MainWindow::generateSample);
        leftLayout->addWidget(sampleBtn);

        QPushButton *loadBtn = new QPushButton("Загрузить файл");
        connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadFile);
        leftLayout->addWidget(loadBtn);

        QPushButton *clearBtn = new QPushButton("Очистить");
        connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clear);
        leftLayout->addWidget(clearBtn);

        QPushButton *exportBtn = new QPushButton("Экспорт");
        connect(exportBtn, &QPushButton::clicked, this, &MainWindow::exportResults);
        leftLayout->addWidget(exportBtn);

        // Статистика
        leftLayout->addWidget(new QLabel("\nСтатистика:"));
        totalLabel = new QLabel("Отрезков: 0");
        visibleLabel = new QLabel("Видимых: 0");
        invisibleLabel = new QLabel("Невидимых: 0");
        leftLayout->addWidget(totalLabel);
        leftLayout->addWidget(visibleLabel);
        leftLayout->addWidget(invisibleLabel);

        leftLayout->addStretch();

        // Область отрисовки
        renderWidget = new RenderWidget;

        mainLayout->addWidget(leftPanel);
        mainLayout->addWidget(renderWidget);

        setCentralWidget(central);

        clipWindow = ClipWindow(100, 100, 400, 300);
    }

private slots:
    void generateSample() {
        lines.clear();
        lines.append(Line(50, 150, 450, 250));
        lines.append(Line(150, 50, 350, 350));
        lines.append(Line(80, 80, 420, 320));
        lines.append(Line(200, 350, 300, 50));
        lines.append(Line(50, 200, 200, 200));
        lines.append(Line(300, 100, 400, 300));
        clipWindow = ClipWindow(100, 100, 400, 300);
        updateView();
    }

    void loadFile() {
        QString filename = QFileDialog::getOpenFileName(this, "Открыть", "", "*.txt");
        if (filename.isEmpty()) return;

        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
            return;
        }

        QTextStream in(&file);
        lines.clear();

        int n;
        in >> n;
        for (int i = 0; i < n; ++i) {
            double x1, y1, x2, y2;
            in >> x1 >> y1 >> x2 >> y2;
            lines.append(Line(x1, y1, x2, y2));
        }

        double xmin, ymin, xmax, ymax;
        in >> xmin >> ymin >> xmax >> ymax;
        clipWindow = ClipWindow(xmin, ymin, xmax, ymax);

        file.close();
        updateView();
        QMessageBox::information(this, "Успех", QString("Загружено: %1").arg(n));
    }

    void clear() {
        lines.clear();
        updateView();
    }

    void exportResults() {
        if (lines.isEmpty()) return;

        QString filename = QFileDialog::getSaveFileName(this, "Сохранить", "results.txt", "*.txt");
        if (filename.isEmpty()) return;

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) return;

        QTextStream out(&file);
        out << "Алгоритм: " << algoCombo->currentText() << "\n\n";

        ClippingAlgorithms clipper(clipWindow);
        for (int i = 0; i < lines.size(); ++i) {
            Line clipped;
            bool visible = false;
            switch (algoCombo->currentIndex()) {
            case 0: visible = clipper.cohenSutherland(lines[i], clipped); break;
            case 1: visible = clipper.liangBarsky(lines[i], clipped); break;
            case 2: visible = clipper.midpointClip(lines[i], clipped); break;
            case 3: visible = clipper.convexPolygonClip(lines[i], clipped); break;
            }
            out << "Отрезок " << (i + 1) << ": " << (visible ? "ВИДИМ" : "НЕВИДИМ") << "\n";
        }

        file.close();
        QMessageBox::information(this, "Успех", "Результаты экспортированы");
    }

    void updateView() {
        renderWidget->setData(lines, clipWindow, algoCombo->currentIndex(),
                              showOriginalCheck->isChecked());

        ClippingAlgorithms clipper(clipWindow);
        int visible = 0;
        for (const Line &line : lines) {
            Line clipped;
            switch (algoCombo->currentIndex()) {
            case 0: if (clipper.cohenSutherland(line, clipped)) visible++; break;
            case 1: if (clipper.liangBarsky(line, clipped)) visible++; break;
            case 2: if (clipper.midpointClip(line, clipped)) visible++; break;
            case 3: if (clipper.convexPolygonClip(line, clipped)) visible++; break;
            }
        }

        totalLabel->setText(QString("Отрезков: %1").arg(lines.size()));
        visibleLabel->setText(QString("Видимых: %1").arg(visible));
        invisibleLabel->setText(QString("Невидимых: %1").arg(lines.size() - visible));
    }

private:
    RenderWidget *renderWidget;
    QComboBox *algoCombo;
    QCheckBox *showOriginalCheck;
    QLabel *totalLabel, *visibleLabel, *invisibleLabel;
    QVector<Line> lines;
    ClipWindow clipWindow;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
