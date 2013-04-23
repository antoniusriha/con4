#-------------------------------------------------
#
# Project created by QtCreator 2013-04-16T17:20:05
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = con4net
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
	initiatorservice.cpp \
	indexservice.cpp \
	opponentservice.cpp \
	networkplayerservice.cpp \
	../con4core/player.cpp \
	../con4core/game.cpp \
	../con4core/board.cpp \
	indexservicelist.cpp \
	../con4core/settings.cpp \
	networkgame.cpp \
    networkgamelist.cpp \
    ngltreeitem.cpp \
    message.cpp \
    endpoint.cpp \
    serverendpoint.cpp \
    clientendpoint.cpp \
    messages.cpp

HEADERS += \
	initiatorservice.h \
	indexservice.h \
	opponentservice.h \
	networkplayerservice.h \
	con4netglobals.h \
	../con4core/player.h \
	../con4core/game.h \
	../con4core/board.h \
	indexservicelist.h \
	../con4core/settings.h \
	networkgame.h \
    networkgamelist.h \
    ngltreeitem.h \
    message.h \
    endpoint.h \
    messages.h \
    serverendpoint.h \
    clientendpoint.h
