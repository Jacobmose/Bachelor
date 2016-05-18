#include "mainwindow.h"
#include <QApplication>

#include <QFile>

#define STYLE_SHEET_PATH ":/qss/stylesheet.qss"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QFile styleFile(STYLE_SHEET_PATH);
        styleFile.open(QFile::ReadOnly);

    if (styleFile.isOpen())
    {
        application.setStyleSheet(styleFile.readAll());
    }

    styleFile.close();

    MainWindow mainWindow;
    mainWindow.show();

    return application.exec();
}
