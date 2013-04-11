#-------------------------------------------------
#
# Project created by QtCreator 2013-04-03T10:32:13
#
#-------------------------------------------------

QT       += core gui network

TARGET = con4
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mythread.cpp \
    gameconfview.cpp \
    joingamesetupview.cpp \
    newgamesetupview.cpp \
    game.cpp \
    aiplayer.cpp \
    board.cpp \
    service.cpp \
    opponentservice.cpp \
    initiatorservice.cpp \
    player.cpp \
    indexservice.cpp \
    addindexserverdialog.cpp \
    application.cpp \
    indexserversview.cpp \
    indexserversviewmodel.cpp \
    networkplayerservice.cpp

HEADERS  += mainwindow.h \
    mythread.h \
    gameconfview.h \
    joingamesetupview.h \
    newgamesetupview.h \
    game.h \
    aiplayer.h \
    board.h \
    service.h \
    opponentservice.h \
    initiatorservice.h \
    player.h \
    indexservice.h \
    addindexserverdialog.h \
    application.h \
    indexserversview.h \
    indexserversviewmodel.h \
    networkplayerservice.h \
    con4globals.h

FORMS    += mainwindow.ui \
    gameconfview.ui \
    joingamesetupview.ui \
    newgamesetupview.ui \
    addindexserverdialog.ui \
    indexserversview.ui
