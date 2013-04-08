#-------------------------------------------------
#
# Project created by QtCreator 2013-04-06T21:52:42
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = testapp
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../src/con4/service.cpp \
    ../../src/con4/networkgame.cpp \
    ../../src/con4/indexservice.cpp \
    ../../src/con4/game.cpp \
    indexservicetest.cpp

HEADERS += \
    ../../src/con4/service.h \
    ../../src/con4/networkgame.h \
    ../../src/con4/indexservice.h \
    ../../src/con4/game.h \
    indexservicetest.h
