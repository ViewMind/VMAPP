QT -= gui
QT += sql

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
    ../../CommonClasses/SQLConn/dbinterface.cpp \
    ../../CommonClasses/LocalInformationManager/localinformationmanager.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp

HEADERS += \
    ../../CommonClasses/server_defines.h \
    ../../CommonClasses/SQLConn/dbdescription.h \
    ../../CommonClasses/SQLConn/dbinterface.h \
    ../../CommonClasses/LocalInformationManager/localinformationmanager.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    ../../CommonClasses/common.h

RESOURCES += \
    dbconf.qrc
