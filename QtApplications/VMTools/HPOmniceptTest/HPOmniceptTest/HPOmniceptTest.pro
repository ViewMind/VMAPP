QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../CommonClasses/EyeTrackingInterface/HPReverb/hpomnicentprovider.cpp \
        main.cpp \
        receiver.cpp

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

LIBS += -L$$PWD/../../../CommonClasses/EyeTrackingInterface/HPReverb/hp -lhp_omnicept
LIBS += -lWinTrust
LIBS += -lRpcRT4
LIBS += -lole32
LIBS += -lshell32
LIBS += -ladvapi32

HEADERS += \
    ../../../CommonClasses/EyeTrackingInterface/HPReverb/hpomnicentprovider.h \
    receiver.h



