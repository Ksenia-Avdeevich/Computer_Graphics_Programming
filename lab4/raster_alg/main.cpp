#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("Raster Algorithms Visualizer");
    window.showMaximized();
    window.show();
    return app.exec();
}
