#-------------------------------------------------
#
# Project created by QtCreator 2018-05-12T18:39:39
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SSLServer
TEMPLATE = app


OBJECTS_DIR = OBJS
MOC_DIR     = MOCS
RCC_DIR     = QRC
CONFIG     -= debug_and_release

SOURCES += main.cpp\
        sslserverwindow.cpp \
    ssllistener.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    sslmanager.cpp \
    ../../CommonClasses/DataPacket/datapacket.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    sslidsocket.cpp

HEADERS  += sslserverwindow.h \
    ../../CommonClasses/common.h \
    ssllistener.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    sslmanager.h \
    ../../CommonClasses/DataPacket/datapacket.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    sslidsocket.h

FORMS    += sslserverwindow.ui

RESOURCES += \
    certificates.qrc
