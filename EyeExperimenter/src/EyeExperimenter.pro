QT += quick gui widgets
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

RESOURCES += qml.qrc \
    fonts.qrc \
    images.qrc \
    languages.qrc

SOURCES += main.cpp \
    loader.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp

HEADERS += \
    loader.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/common.h
