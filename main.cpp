#include "mainwindow.h"
#include <QApplication>

#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QFile styleFile("C:/Projects/QtCreator/Bachelor/Bachelor/style.qss");
//    styleFile.open(QFile::ReadOnly);
//    QString styleSheet = QLatin1String(styleFile.readAll());
//    a.setStyleSheet(styleSheet);

    MainWindow w;
    w.show();

    return a.exec();
}
