#-------------------------------------------------
#
# Project created by QtCreator 2018-10-28T10:28:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LocalDBMng
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        dbview.cpp \
    ../../CommonClasses/LocalInformationManager/localinformationmanager.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.cpp \
    howmanydiag.cpp

HEADERS += \
        dbview.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/LocalInformationManager/localinformationmanager.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.h \
    howmanydiag.h

FORMS += \
        dbview.ui \
    howmanydiag.ui
