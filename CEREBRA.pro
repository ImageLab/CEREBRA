#-------------------------------------------------
#
# Project created by QtCreator 2015-02-10T11:05:15
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CEREBRA
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    packetrendererglwidget.cpp

HEADERS  += mainwindow.h \
    packetrendererglwidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    shaderresource.qrc

unix|win32: LIBS += -L$$PWD/ -llibsimple

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/libsimple.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/liblibsimple.a
