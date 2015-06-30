QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CEREBRA
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    packetrendererglwidget.cpp \
    packetfilereader.cpp

HEADERS  += mainwindow.h \
    packetrendererglwidget.h \
    packetfilereader.h \
    objparser.h \
    brain_cover.h \
    libmatiohelper.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

LIBS += -L"$$_PRO_FILE_PWD_/lib/x64" -llibsimple
LIBS += -L"$$_PRO_FILE_PWD_/lib/x64" -llibmatio
