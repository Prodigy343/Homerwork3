#-------------------------------------------------
#
# Project created by QtCreator 2016-09-15T20:27:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = BMPEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    histogram.cpp \
    bmpfile.cpp \
    qcustomplot.cpp \
    customfilter.cpp

HEADERS  += mainwindow.h \
    histogram.h \
    bmpfile.h \
    qcustomplot.h \
    customfilter.h

FORMS    += mainwindow.ui \
    histogram.ui \
    customfilter.ui
