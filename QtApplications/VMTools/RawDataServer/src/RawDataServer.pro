QT -= gui
QT += core  network sql

CONFIG += c++11 console
CONFIG -= app_bundle

OBJECTS_DIR = OBJS
MOC_DIR     = MOCS
RCC_DIR     = QRC
CONFIG     -= debug_and_release
CONFIG     += console

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    s3interface.cpp \
    ../../../CommonClasses/LogInterface/loginterface.cpp \
    control.cpp \
    ../../../CommonClasses/SQLConn/dbinterface.cpp \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ssllistener.cpp \
    ../../../CommonClasses/DataPacket/datapacket.cpp \
    filelister.cpp \
    ../../../CommonClasses/LocalInformationManager/localinformationmanager.cpp \
    ../../../CommonClasses/DatFileInfo/datfileinfoindir.cpp \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocket.cpp \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.cpp \
    ../../../CommonClasses/VariantMapSerializer/variantmapserializer.cpp \
    rawdataserversocket.cpp

HEADERS += \
    s3interface.h \
    ../../../CommonClasses/server_defines.h \
    ../../../CommonClasses/LogInterface/loginterface.h \
    control.h \
    ../../../CommonClasses/SQLConn/dbdescription.h \
    ../../../CommonClasses/SQLConn/dbinterface.h \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ssllistener.h \
    ../../../CommonClasses/DataPacket/datapacket.h \
    filelister.h \
    ../../../CommonClasses/server_defines.h \
    ../../../CommonClasses/common.h \
    ../../../CommonClasses/LocalInformationManager/localinformationmanager.h \
    ../../../CommonClasses/DatFileInfo/datfileinfoindir.h \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocket.h \
    ../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.h \
    ../../../CommonClasses/VariantMapSerializer/variantmapserializer.h \
    rawdataserversocket.h

RESOURCES += \
    certificates.qrc

