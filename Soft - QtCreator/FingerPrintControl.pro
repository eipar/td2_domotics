#-------------------------------------------------
#
# Project created by QtCreator 2015-06-23T10:33:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FingerPrintControl
TEMPLATE = app

include(qextserialport-1.2rc/src/qextserialport.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    dialog_admin.cpp \
    dialog_user.cpp \
    dialog_forgot.cpp

HEADERS  += mainwindow.h \
    dialog_admin.h \
    dialog_user.h \
    dialog_forgot.h

FORMS    += mainwindow.ui \
    dialog_admin.ui \
    dialog_user.ui \
    dialog_forgot.ui
