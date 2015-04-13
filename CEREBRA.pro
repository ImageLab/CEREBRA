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
    packetfilereader.h

FORMS    += mainwindow.ui

RESOURCES += \
    shaderresource.qrc

unix|win32: LIBS += -L$$PWD/lib -llibsimple

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/libsimple.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/lib/liblibsimple.a

unix|win32: LIBS += -LC:/MATLAB/R2014/extern/lib/win32/microsoft/ -llibeng

INCLUDEPATH += $$PWD/lib/extern/include
DEPENDPATH += $$PWD/lib/extern/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/extern/lib/win32/microsoft/libeng.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/lib/extern/lib/win32/microsoft/liblibeng.a

unix|win32: LIBS += -L$$PWD/lib/extern/lib/win32/microsoft/ -llibmat

INCLUDEPATH += $$PWD/lib/extern/include
DEPENDPATH += $$PWD/lib/extern/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/extern/lib/win32/microsoft/libmat.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/lib/extern/lib/win32/microsoft/liblibmat.a

INCLUDEPATH += $$PWD/lib/extern/include
DEPENDPATH += $$PWD/lib/extern/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/extern/lib/win32/microsoft/libmat.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/lib/extern/lib/win32/microsoft/liblibmat.a
