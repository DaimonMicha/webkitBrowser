#-------------------------------------------------
#
# Project created by QtCreator 2016-05-24T03:45:25
#
#-------------------------------------------------

TEMPLATE      = lib
CONFIG       += plugin

include(../global.pri)

QT           += widgets network sql webkit webkitwidgets

DESTDIR       = ../
TARGET        = battleknight

SOURCES      += \
        battleknight.cpp

HEADERS      += \
        battleknight.h

