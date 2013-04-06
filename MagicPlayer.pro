#-------------------------------------------------
#
# Project created by QtCreator 2013-04-04T14:51:00
#
#-------------------------------------------------

QT       += core gui

TARGET = MagicPlayer
TEMPLATE = app


SOURCES += main.cpp\
        magicplayer.cpp

HEADERS  += magicplayer.h

FORMS    += magicplayer.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/release/ -lvlc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/debug/ -lvlc
else:unix:!symbian: LIBS += -L$$PWD/../../../../usr/lib/ -lvlc

INCLUDEPATH += $$PWD/../../../../usr/include
DEPENDPATH += $$PWD/../../../../usr/include
