#-------------------------------------------------
#
# Project created by QtCreator 2016-05-04T13:06:56
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bachelor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    jogdialog.cpp \
    browsefigureswidget.cpp \
    filedialog.cpp \
    jogwindow.cpp \
    printobject.cpp

HEADERS  += mainwindow.h \
    jogdialog.h \
    browsefigureswidget.h \
    filedialog.h \
    jogwindow.h \
    printobject.h

RESOURCES += \
    resources.qrc

FORMS    += mainwindow.ui \
    jogdialog.ui \
    filedialog.ui \
    jogwindow.ui
