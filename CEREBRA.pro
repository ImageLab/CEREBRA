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
    packetrendererglwidget.cpp \
    packetfilereader.cpp

HEADERS  += mainwindow.h \
    packetrendererglwidget.h \
    packetfilereader.h \
    objparser.h \
    brain_cover.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

INCLUDEPATH += $$PWD/lib/extern/include
DEPENDPATH += $$PWD/lib/extern/include

LIBS += -lglew32 -lglu32 -lopengl32
LIBS += -L"$$_PRO_FILE_PWD_/lib" -llibsimple
LIBS += -L"$$_PRO_FILE_PWD_/lib/extern/lib/win32/microsoft" -llibeng
LIBS += -L"$$_PRO_FILE_PWD_/lib/extern/lib/win32/microsoft" -llibmat
LIBS += -L"$$_PRO_FILE_PWD_/lib/extern/lib/win32/microsoft" -llibmx
