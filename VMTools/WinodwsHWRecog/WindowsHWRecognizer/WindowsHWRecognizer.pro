QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../CommonClasses/HWRecog/hwrecognizer.cpp \
        ../../../CommonClasses/HWRecog/processrecognizer.cpp \
        ../../../CommonClasses/HWRecog/tableoutputparser.cpp \
        main.cpp

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../../CommonClasses/HWRecog/hwrecognizer.h \
    ../../../CommonClasses/HWRecog/processrecognizer.h \
    ../../../CommonClasses/HWRecog/tableoutputparser.h
