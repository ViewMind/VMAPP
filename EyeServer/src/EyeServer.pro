#-------------------------------------------------
#
# Project created by QtCreator 2018-05-12T18:39:39
#
#-------------------------------------------------

QT       += core network sql
TARGET   = EyeServer
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
    sslidsocket.cpp \
    servercontrol.cpp \
    ../../CommonClasses/SQLConn/dbinterface.cpp \
    dataprocessingsslserver.cpp \
    dbcommsslserver.cpp

HEADERS  += \
    ../../CommonClasses/common.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/DataPacket/datapacket.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    ssllistener.h \
    sslidsocket.h \
    servercontrol.h \
    ../../CommonClasses/SQLConn/dbdescription.h \
    ../../CommonClasses/SQLConn/dbinterface.h \
    dataprocessingsslserver.h \
    dbcommsslserver.h \
    ../../CommonClasses/server_defines.h

RESOURCES += \
    certificates.qrc \
    configurations.qrc
