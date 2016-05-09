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
    devicedialog.cpp \
    figuredialog.cpp \
    confirmprintdialog.cpp

HEADERS  += mainwindow.h \
    serialhandler.h \
    gcodehandler.h \
    devicedialog.h \
    figuredialog.h \
    confirmprintdialog.h

RESOURCES += style.qrc

FORMS    += mainwindow.ui \
    devicedialog.ui \
    figuredialog.ui \
    confirmprintdialog.ui
