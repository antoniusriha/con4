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
    networkproxy.cpp \
    indexservice.cpp \
    networkgame.cpp \
    service.cpp \
    opponentservice.cpp \
    initiatorservice.cpp \
    gameinterface.cpp

HEADERS  += mainwindow.h \
    mythread.h \
    gameconfview.h \
    joingamesetupview.h \
    newgamesetupview.h \
    game.h \
    aiplayer.h \
    board.h \
    networkproxy.h \
    indexservice.h \
    networkgame.h \
    service.h \
    opponentservice.h \
    initiatorservice.h \
    gameinterface.h

FORMS    += mainwindow.ui \
    gameconfview.ui \
    joingamesetupview.ui \
    newgamesetupview.ui
