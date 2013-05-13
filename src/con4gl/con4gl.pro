#-------------------------------------------------
#
# Project created by QtCreator 2013-04-14T10:21:19
#
#-------------------------------------------------

QT       += core gui opengl

LIBS = -lGLU

TARGET = con4gl
TEMPLATE = app


SOURCES += main.cpp \
    grid.cpp \
    ../con4core/game.cpp \
    ../con4core/board.cpp \
    disks.cpp \
    boardconf.cpp \
    boardview.cpp

HEADERS  += \
    grid.h \
    ../con4core/game.h \
    ../con4core/board.h \
    disks.h \
    boardconf.h \
    boardview.h

FORMS    += \
    boardview.ui
