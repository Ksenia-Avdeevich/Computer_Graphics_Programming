/********************************************************************************
** Form generated from reading UI file 'colorconverterapp.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLORCONVERTERAPP_H
#define UI_COLORCONVERTERAPP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ColorConverterApp
{
public:

    void setupUi(QWidget *ColorConverterApp)
    {
        if (ColorConverterApp->objectName().isEmpty())
            ColorConverterApp->setObjectName("ColorConverterApp");
        ColorConverterApp->resize(800, 600);

        retranslateUi(ColorConverterApp);

        QMetaObject::connectSlotsByName(ColorConverterApp);
    } // setupUi

    void retranslateUi(QWidget *ColorConverterApp)
    {
        ColorConverterApp->setWindowTitle(QCoreApplication::translate("ColorConverterApp", "ColorConverterApp", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ColorConverterApp: public Ui_ColorConverterApp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLORCONVERTERAPP_H
