#-------------------------------------------------
#
# Project created by QtCreator 2013-04-05T19:35:03
#
#-------------------------------------------------

QT       += core gui testlib network

TARGET = con4test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    tst_board.cpp \
    ../../src/con4/board.cpp \
	tst_indexservice.cpp \
    ../../src/con4/indexservice.cpp \
    main.cpp \
    ../../src/con4/service.cpp \
    ../../src/con4/networkgame.cpp \
    ../../src/con4/game.cpp \
    ../../src/con4/opponentservice.cpp \
    ../../src/con4/initiatorservice.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    tst_global.h \
    ../../src/con4/service.h \
    ../../src/con4/indexservice.h \
    ../../src/con4/networkgame.h \
    ../../src/con4/game.h \
    ../../src/con4/opponentservice.h \
    ../../src/con4/initiatorservice.h \
    ../../src/con4/board.h
