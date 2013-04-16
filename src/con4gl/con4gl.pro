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
    glwidget.cpp \
    glelement.cpp \
    grid.cpp \
    ../con4core/game.cpp \
    ../con4core/board.cpp

HEADERS  += \
    glwidget.h \
    glelement.h \
    grid.h \
    ../con4core/game.h \
    ../con4core/board.h

FORMS    +=
