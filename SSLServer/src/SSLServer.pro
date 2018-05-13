#-------------------------------------------------
#
# Project created by QtCreator 2018-05-12T18:39:39
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SSLServer
TEMPLATE = app

MOC_DIR = MOCS
OBJECTS_DIR = OBJS

SOURCES += main.cpp\
        sslserverwindow.cpp \
    ssllistener.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    sslmanager.cpp \
    ../../CommonClasses/DataPacket/datapacket.cpp \
    ../../CommonClasses/SSLIDSocket/sslidsocket.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp

HEADERS  += sslserverwindow.h \
    ../../CommonClasses/common.h \
    ssllistener.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    sslmanager.h \
    ../../CommonClasses/DataPacket/datapacket.h \
    ../../CommonClasses/SSLIDSocket/sslidsocket.h \
    ../../CommonClasses/LogInterface/loginterface.h

FORMS    += sslserverwindow.ui

RESOURCES += \
    certificates.qrc
