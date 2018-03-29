#-------------------------------------------------
#
# Project created by QtCreator 2018-01-31T20:31:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network serialport printsupport

TARGET = dronefly
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    autoint.cpp \
    calscan.cpp \
    myinterfacekit.cpp \
    mysocket.cpp \
    myspectrometer.cpp \
    phidgetsMot.cpp \
    processso2.cpp \
    qgpsdevice.cpp \
    repeatScan.cpp \
    repeatscandialog.cpp \
    scanthread.cpp \
    polyfitd/poly.cpp \
    polyfitd/nrutil.c \
    polyfitd/svbksb.c \
    polyfitd/svbksb_d.c \
    polyfitd/svdcmp.c \
    polyfitd/svdcmp_d.c \
    polyfitd/svdfit.c \
    polyfitd/svdfit_d.c \
    polyfitd/svdvar.c

HEADERS  += mainwindow.h \
    autoint.h \
    calscan.h \
    myinterfacekit.h \
    mysocket.h \
    myspectrometer.h \
    phidgetsMot.h \
    processso2.h \
    qgpsdevice.h \
    repeatScan.h \
    repeatscandialog.h \
    scanthread.h \
    polyfitd/nr.h \
    polyfitd/nrutil.h

FORMS    += mainwindow.ui \
    repeatscandialog.ui

INCLUDEPATH += /home/pi/workdir/SeaBreeze/include
INCLUDEPATH += ./polyfitd
LIBS += -L/home/pi/workdir/SeaBreeze/lib -lseabreeze -lphidget21 -lusb
