#-------------------------------------------------
#
# Project created by QtCreator 2018-05-12T18:39:39
#
#-------------------------------------------------

QT       += core network
TARGET   = EyeServer
TEMPLATE = app

OBJECTS_DIR = OBJS
MOC_DIR     = MOCS
RCC_DIR     = QRC
CONFIG     -= debug_and_release
CONFIG     += console

SOURCES += main.cpp\
    ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../../CommonClasses/DataPacket/datapacket.cpp \
    ../../../CommonClasses/LogInterface/loginterface.cpp \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocket.cpp \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.cpp \
    ssllistener.cpp \
    servercontrol.cpp \    
    dataprocessingsslserver.cpp \
    ../../../CommonClasses/TimeMeasurer/timemeasurer.cpp \
    processingsocket.cpp


HEADERS  += \
    ../../../CommonClasses/common.h \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../../CommonClasses/DataPacket/datapacket.h \
    ../../../CommonClasses/LogInterface/loginterface.h \
    ../../../CommonClasses/server_defines.h \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocket.h \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.h \
    dataprocessingsslserver.h \
    ssllistener.h \
    servercontrol.h \
    ../../../CommonClasses/TimeMeasurer/timemeasurer.h \
    processingsocket.h

RESOURCES += \
    certificates.qrc \

