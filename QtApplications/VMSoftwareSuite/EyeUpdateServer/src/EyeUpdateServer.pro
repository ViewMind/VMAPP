QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

OBJECTS_DIR = OBJS
MOC_DIR     = MOCS
RCC_DIR     = QRC
CONFIG     -= debug_and_release
CONFIG     += console

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    servercontrol.cpp \
    ssllistener.cpp \
    updateserver.cpp \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../../CommonClasses/DataPacket/datapacket.cpp \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocket.cpp \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.cpp \
    ../../../CommonClasses/LogInterface/loginterface.cpp \
    updatesocket.cpp

RESOURCES += \
    certificates.qrc

HEADERS += \
    servercontrol.h \
    ssllistener.h \
    ../../../CommonClasses/common.h \
    ../../../CommonClasses/server_defines.h \
    updateserver.h \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../../CommonClasses/DataPacket/datapacket.h \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocket.h \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.h \
    ../../../CommonClasses/LogInterface/loginterface.h \
    updatesocket.h
