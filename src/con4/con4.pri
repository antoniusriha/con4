#-------------------------------------------------
#
# Project created by QtCreator 2013-04-03T10:32:13
#
#-------------------------------------------------

INCLUDEPATH += $$system(pwd)
DEPENDPATH += $$system(pwd)

QT       += core gui network opengl

CONFIG += exceptions

LIBS = -lGLU

TARGET = confour
TEMPLATE = app


SOURCES += main.cpp\
		mainwindow.cpp \
	addindexserverdialog.cpp \
	application.cpp \
	indexserversview.cpp \
	../con4gl/grid.cpp \
	../con4core/game.cpp \
	../con4core/board.cpp \
	../con4net/indexservice.cpp \
	../con4net/indexservicelist.cpp \
	../con4core/settings.cpp \
	../con4net/networkgame.cpp \
	../con4net/serverendpoint.cpp \
	../con4net/messages.cpp \
	../con4net/message.cpp \
	../con4net/endpoint.cpp \
	../con4net/clientendpoint.cpp \
	gamehost.cpp \
	indexservicetablemodel.cpp \
	../con4net/netopponent.cpp \
	../con4net/netinitiator.cpp \
	networkgameitemmodel.cpp \
	ngltreeitem.cpp \
	../con4net/processingqueue.cpp \
	networkgameview.cpp \
	playerconfview.cpp \
	colorbutton.cpp \
	validatingtextbox.cpp \
	ipaddresstextbox.cpp \
	boardconfview.cpp \
	localgameconfview.cpp \
	../con4net/indexservicehelper.cpp \
	../con4net/clientendpointhelper.cpp \
	../con4net/endpointhelper.cpp \
	../con4net/indexservicemanager.cpp \
	joingameconfview.cpp \
    ../con4gl/disks.cpp \
    ../con4gl/boardview.cpp \
    ../con4gl/boardconf.cpp \
    randomsetplayer.cpp \
    aiplayerinfo.cpp \
    aiplayer.cpp

HEADERS  += mainwindow.h \
	addindexserverdialog.h \
	application.h \
	indexserversview.h \
	../con4gl/grid.h \
	../con4core/game.h \
	../con4core/board.h \
	../con4net/indexservice.h \
	../con4net/indexservicelist.h \
	../con4core/settings.h \
	../con4net/networkgame.h \
	../con4net/serverendpoint.h \
	../con4net/messages.h \
	../con4net/message.h \
	../con4net/endpoint.h \
	../con4net/clientendpoint.h \
	gamehost.h \
	indexservicetablemodel.h \
	../con4net/netopponent.h \
	../con4net/netinitiator.h \
	networkgameitemmodel.h \
	ngltreeitem.h \
	../con4net/processingqueue.h \
	networkgameview.h \
	playerconfview.h \
	colorbutton.h \
	validatingtextbox.h \
	ipaddresstextbox.h \
	textvalidationcriteria.h \
	boardconfview.h \
	localgameconfview.h \
	../con4net/indexservicehelper.h \
	../con4net/clientendpointhelper.h \
	../con4net/endpointhelper.h \
	../con4net/indexservicemanager.h \
	joingameconfview.h \
    ../con4gl/disks.h \
    ../con4gl/boardview.h \
    ../con4gl/boardconf.h \
    randomsetplayer.h \
    aiplayerinfo.h \
    aiplayer.h

FORMS    += mainwindow.ui \
	addindexserverdialog.ui \
	indexserversview.ui \
	networkgameview.ui \
	playerconfview.ui \
	validatingtextbox.ui \
	boardconfview.ui \
	localgameconfview.ui \
	joingameconfview.ui \
    ../con4gl/boardview.ui

unix {
	SRCDIR = ../..

	target.path = $${PREFIX}/lib/confour
	INSTALLS += target

	system(sed -e "s:\\@pkglibdir\\@:$${PREFIX}/lib/confour:g" $${SRCDIR}/data/con4.in > $${SRCDIR}/data/con4; chmod +x $${SRCDIR}/data/con4)
	wrapper.files += $${SRCDIR}/data/con4
	wrapper.path = $${PREFIX}/bin
	INSTALLS += wrapper
	
	DISTFILES += data/con4.in
}

