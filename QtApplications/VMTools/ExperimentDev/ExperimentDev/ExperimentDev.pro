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
        ../../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptprovider.cpp \
        ../../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptinterface.cpp \
        ../../../CommonClasses/EyeTrackingInterface/Mouse/calibrationarea.cpp \
        ../../../CommonClasses/EyeTrackingInterface/Mouse/mouseinterface.cpp \
        ../../../CommonClasses/EyeTrackingInterface/calibrationleastsquares.cpp \
        ../../../CommonClasses/EyeTrackingInterface/calibrationtargets.cpp \
        ../../../CommonClasses/EyeTrackingInterface/eyetrackerdata.cpp \
        ../../../CommonClasses/EyeTrackingInterface/eyetrackerinterface.cpp \
        ../../../CommonClasses/LogInterface/loginterface.cpp \
        ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.cpp \
        ../../../CommonClasses/QMLQImageDisplay/qimagedisplay.cpp \
        control.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptprovider.h \
    ../../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptinterface.h \
    ../../../CommonClasses/EyeTrackingInterface/Mouse/calibrationarea.h \
    ../../../CommonClasses/EyeTrackingInterface/Mouse/mouseinterface.h \
    ../../../CommonClasses/EyeTrackingInterface/calibrationleastsquares.h \
    ../../../CommonClasses/EyeTrackingInterface/calibrationtargets.h \
    ../../../CommonClasses/EyeTrackingInterface/eyetrackerdata.h \
    ../../../CommonClasses/EyeTrackingInterface/eyetrackerinterface.h \
    ../../../CommonClasses/LogInterface/loginterface.h \
    ../../../CommonClasses/OpenVRControlObject/openvr.h \
    ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h \
    ../../../CommonClasses/QMLQImageDisplay/qimagedisplay.h \
    ../../../CommonClasses/RawDataContainer/VMDC.h \
    control.h

LIBS += -L$$PWD/../../../CommonClasses/OpenVRControlObject/libs -lopenvr_api
LIBS += -L$$PWD/../../../CommonClasses/EyeTrackingInterface/HPReverb/hp -lhp_omnicept
LIBS += -lWinTrust
LIBS += -lRpcRT4
LIBS += -lole32
LIBS += -lshell32
LIBS += -ladvapi32
