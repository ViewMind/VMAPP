QT -= gui
QT += sql widgets

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
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
    consoleinputscreen.cpp \
    control.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    InstDBComm.cpp \
    ../../CommonClasses/SQLConn/dbinterface.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp

HEADERS += \
    consoleinputscreen.h \
    control.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/SQLConn/dbdescription.h \
    InstDBComm.h \
    ../../CommonClasses/SQLConn/dbinterface.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/LogInterface/loginterface.h
