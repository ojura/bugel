#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T07:26:42
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bugel
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    timelinelayerwidget.cpp \
    timeline.cpp \
    timelinecontainer.cpp \
    timelinewidget.cpp \
    playbackwidget.cpp \
    util.cpp \
    timelineeventswidget.cpp \
    timelinesettingsdialog.cpp \
    luascriptcontext.cpp \
    timelineevent.cpp \
    timelinelayer.cpp \
    eventlist.cpp \
    bugelexception.cpp \
    placeholderevent.cpp \
    project.cpp \
    projectsettingsdialog.cpp \
    eventtypemodel.cpp

HEADERS  += mainwindow.h \
    timelinelayerwidget.h \
    timeline.h \
    timelinecontainer.h \
    timelinewidget.h \
    playbackwidget.h \
    util.h \
    timelineeventswidget.h \
    selection.h \
    timelinesettingsdialog.h \
    luascriptcontext.h \
    timelineevent.h \
    timelinelayer.h \
    eventlist.h \
    bugelexception.h \
    placeholderevent.h \
    project.h \
    projectsettingsdialog.h \
    eventtypemodel.h

FORMS    += mainwindow.ui \
    timelinelayerwidget.ui \
    timelinecontainer.ui \
    playbackwidget.ui \
    timelinesettingsdialog.ui \
    projectsettingsdialog.ui

INCLUDEPATH += /usr/include/lua5.2
LIBS += /usr/lib/x86_64-linux-gnu/liblua5.2.so

INCLUDEPATH += LuaState/include

QMAKE_CXXFLAGS += -D_CRT_SECURE_NO_WARNINGS
