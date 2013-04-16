#-------------------------------------------------
#
# Project created by QtCreator 2013-04-03T10:32:13
#
#-------------------------------------------------

QT       += core gui network opengl

LIBS = -lGLU

TARGET = con4
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aiplayer.cpp \
    service.cpp \
    opponentservice.cpp \
    initiatorservice.cpp \
    indexservice.cpp \
    addindexserverdialog.cpp \
    application.cpp \
    indexserversview.cpp \
    indexserversviewmodel.cpp \
    networkplayerservice.cpp \
    ../con4gl/glwidget.cpp \
    ../con4gl/glelement.cpp \
    ../con4gl/grid.cpp \
    ../con4core/game.cpp \
    ../con4core/board.cpp \
    settingsdialog.cpp \
    boardconf.cpp

HEADERS  += mainwindow.h \
    aiplayer.h \
    service.h \
    opponentservice.h \
    initiatorservice.h \
    indexservice.h \
    addindexserverdialog.h \
    application.h \
    indexserversview.h \
    indexserversviewmodel.h \
    networkplayerservice.h \
    con4globals.h \
    ../con4gl/glwidget.h \
    ../con4gl/glelement.h \
    ../con4gl/grid.h \
    ../con4core/game.h \
    ../con4core/board.h \
    settingsdialog.h \
    boardconf.h

FORMS    += mainwindow.ui \
    addindexserverdialog.ui \
    indexserversview.ui \
    settingsdialog.ui \
    boardconf.ui
