#-------------------------------------------------
#
# Project created by QtCreator 2013-04-03T19:22:19
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_board
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    tst_board.cpp \
    ../../src/libcon4/board.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
