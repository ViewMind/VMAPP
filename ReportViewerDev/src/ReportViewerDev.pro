#-------------------------------------------------
#
# Project created by QtCreator 2018-05-19T09:24:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ReportViewerDev
TEMPLATE = app

MOC_DIR = MOCS
OBJECTS_DIR = OBJS

SOURCES += main.cpp\
        reportviewerdev.cpp \
    ../../CommonClasses/ImageExplorer/imageexplorer.cpp

HEADERS  += reportviewerdev.h \
    ../../CommonClasses/ImageExplorer/imageexplorer.h

FORMS    += reportviewerdev.ui
