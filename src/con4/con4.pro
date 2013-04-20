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
    addindexserverdialog.cpp \
    application.cpp \
    indexserversview.cpp \
    ../con4gl/glwidget.cpp \
    ../con4gl/glelement.cpp \
    ../con4gl/grid.cpp \
    ../con4core/game.cpp \
    ../con4core/board.cpp \
    settingsdialog.cpp \
    boardconf.cpp \
    ../con4core/player.cpp \
    ../con4net/indexservice.cpp \
    ../con4net/opponentservice.cpp \
    ../con4net/networkplayerservice.cpp \
    ../con4net/initiatorservice.cpp \
    ../con4net/indexservicelist.cpp \
    ../con4core/settings.cpp \
    ../con4net/networkgame.cpp \
    ../con4net/networkgamelist.cpp \
    ../con4net/ngltreeitem.cpp

HEADERS  += mainwindow.h \
    aiplayer.h \
    addindexserverdialog.h \
    application.h \
    indexserversview.h \
    ../con4gl/glwidget.h \
    ../con4gl/glelement.h \
    ../con4gl/grid.h \
    ../con4core/game.h \
    ../con4core/board.h \
    settingsdialog.h \
    boardconf.h \
    ../con4core/player.h \
    ../con4net/indexservice.h \
    ../con4net/opponentservice.h \
    ../con4net/networkplayerservice.h \
    ../con4net/initiatorservice.h \
    ../con4net/con4netglobals.h \
    ../con4net/indexservicelist.h \
    ../con4core/settings.h \
    ../con4net/networkgame.h \
    ../con4net/networkgamelist.h \
    ../con4net/ngltreeitem.h

FORMS    += mainwindow.ui \
    addindexserverdialog.ui \
    indexserversview.ui \
    settingsdialog.ui \
    boardconf.ui
