TEMPLATE = app
CONFIG += console
CONFIG -= qt

unix:!macx:!symbian: LIBS += -L/usr/lib/ -lIrrlicht

INCLUDEPATH += /usr/include/irrlicht
DEPENDPATH += /usr/include/irrlicht

HEADERS +=  \
    example1.h \
    example2.h

SOURCES += \
    program.cpp

unix:!macx:!symbian: LIBS += -L$$PWD/../../Downloads/tokamakp/trunk/tokamaksrc/ -ltokamak

INCLUDEPATH += $$PWD/../../Downloads/tokamakp/trunk/include
DEPENDPATH += $$PWD/../../Downloads/tokamakp/trunk/include
