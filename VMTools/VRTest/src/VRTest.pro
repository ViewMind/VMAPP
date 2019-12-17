QT += quick widgets gui opengl

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../CommonClasses/LogInterface/loginterface.cpp \
        ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.cpp \
        control.cpp \
        eyetrackerdata.cpp \
        main.cpp \
        targettest.cpp \
        viveeyepoller.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

## Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../../CommonClasses/LogInterface/loginterface.h \
    ../../../CommonClasses/OpenVRControlObject/openvr.h \
    ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h \
    control.h \
    eyetrackerdata.h \
    sranipal/SRanipal.h \
    sranipal/SRanipal_Enums.h \
    sranipal/SRanipal_Eye.h \
    sranipal/SRanipal_EyeDataType.h \
    sranipal/SRanipal_EyeData_v1.h \
    sranipal/SRanipal_EyeData_v2.h \
    sranipal/SRanipal_Eye_Calibration_Enums.h \
    sranipal/SRanipal_Lip.h \
    sranipal/SRanipal_LipData_v1.h \
    sranipal/SRanipal_LipData_v2.h \
    sranipal/ViveSR_Enums.h \
    targettest.h \
    viveeyepoller.h

LIBS += -L$$PWD/lib -lSRanipal
LIBS += -L$$PWD/../../../CommonClasses/OpenVRControlObject/libs -lopenvr_api
