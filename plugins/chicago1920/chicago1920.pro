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
TARGET        = chicago1920

SOURCES      += \
        chicago1920.cpp \
        chaccount.cpp \
    heistworker.cpp \
    fightworker.cpp \
    chplayertable.cpp \
    infoworker.cpp \
    settings.cpp \
    traitorworker.cpp

HEADERS      += \
        chicago1920.h \
        chaccount.h \
    heistworker.h \
    fightworker.h \
    chplayertable.h \
    infoworker.h \
    settings.h \
    traitorworker.h

OTHER_FILES += \
    htmls/inject.html \
    htmls/checkscript.js

RESOURCES += \
    htmls/data.qrc

FORMS += \
    settings.ui

