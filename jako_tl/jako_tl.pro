#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T11:44:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = jako_tl
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bazmek.cpp \
    bazmekarium.cpp \
    darwin.cpp \
    darwin2.cpp

HEADERS  += mainwindow.h \
    bazmek.h \
    bazmekarium.h \
    darwin.h \
    darwin2.h

FORMS    += mainwindow.ui

CONFIG += c++14
