# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = app
TARGET = simplemazegen
DESTDIR = ../
QT += core gui
CONFIG += release
DEFINES += QT_LARGEFILE_SUPPORT
INCLUDEPATH += ./GeneratedFiles \
    ./GeneratedFiles/Release \
    .
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
HEADERS += ./simplemazegen.h \
    ./maze.h
SOURCES += ./main.cpp \
    ./maze.cpp \
    ./simplemazegen.cpp
FORMS += ./maze.ui \
    ./simplemazegen.ui
RESOURCES += simplemazegen.qrc