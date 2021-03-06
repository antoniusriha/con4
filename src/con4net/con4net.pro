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
	../con4core/game.cpp \
	../con4core/board.cpp \
	../con4core/settings.cpp \
	networkgame.cpp \
	message.cpp \
	endpoint.cpp \
	messages.cpp \
	processingqueue.cpp \
	clientendpoint.cpp \
	serverendpoint.cpp \
	indexservice.cpp \
	indexservicelist.cpp \
    netinitiator.cpp \
    netopponent.cpp \
    indexservicehelper.cpp \
    clientendpointhelper.cpp \
    endpointhelper.cpp \
    indexservicemanager.cpp

HEADERS += \
	../con4core/game.h \
	../con4core/board.h \
	../con4core/settings.h \
	networkgame.h \
	message.h \
	endpoint.h \
	messages.h \
	processingqueue.h \
	clientendpoint.h \
	serverendpoint.h \
	indexservice.h \
	indexservicelist.h \
    netinitiator.h \
    netopponent.h \
    indexservicehelper.h \
    clientendpointhelper.h \
    endpointhelper.h \
    indexservicemanager.h
