#include <QApplication>
#include "imageeditor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ImageEditor editor;
    editor.setWindowTitle(QStringLiteral("Редактор изображений"));
    editor.showMaximized();
    editor.show();
    return app.exec();
}
