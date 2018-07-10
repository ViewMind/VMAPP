#-------------------------------------------------
#
# Project created by QtCreator 2018-05-12T18:39:39
#
#-------------------------------------------------

QT       += core widgets network sql
TARGET   = SSLServer
TEMPLATE = app


OBJECTS_DIR = OBJS
MOC_DIR     = MOCS
RCC_DIR     = QRC
CONFIG     -= debug_and_release
CONFIG     += console

SOURCES += main.cpp\
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/DataPacket/datapacket.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    ssllistener.cpp \
    sslmanager.cpp \
    sslidsocket.cpp \
    inputcontrol.cpp \
    servercontrol.cpp \
    dbcommmanager.cpp \
    ../../CommonClasses/SQLConn/dbinterface.cpp

HEADERS  += \
    ../../CommonClasses/common.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/DataPacket/datapacket.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    ssllistener.h \
    sslmanager.h \
    sslidsocket.h \
    inputcontrol.h \
    servercontrol.h \
    ../../CommonClasses/SQLConn/dbdescription.h \
    dbcommmanager.h \
    ../../CommonClasses/SQLConn/dbinterface.h

RESOURCES += \
    certificates.qrc
