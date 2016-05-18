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
    serialhandler.cpp \
    gcodehandler.cpp \
    jogdialog.cpp \
    browsefigureswidget.cpp \
    filedialog.cpp \
    sliceandprintdialog.cpp

HEADERS  += mainwindow.h \
    serialhandler.h \
    gcodehandler.h \
    jogdialog.h \
    browsefigureswidget.h \
    filedialog.h \
    sliceandprintdialog.h

RESOURCES += \
    resources.qrc

FORMS    += mainwindow.ui \
    jogdialog.ui \
    filedialog.ui \
    sliceandprintdialog.ui
