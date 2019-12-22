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
        ../../../CommonClasses/CalibrationLeastSquares/calibrationleastsquares.cpp \
        ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
        ../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.cpp \
        ../../../CommonClasses/Experiments/bindingmanager.cpp \
        ../../../CommonClasses/Experiments/bindingparser.cpp \
        ../../../CommonClasses/Experiments/experimentdatapainter.cpp \
        ../../../CommonClasses/Experiments/fieldingmanager.cpp \
        ../../../CommonClasses/Experiments/readingmanager.cpp \
        ../../../CommonClasses/Experiments/readingparser.cpp \
        ../../../CommonClasses/LogInterface/loginterface.cpp \
        ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.cpp \
        ../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/experiment.cpp \
        ../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/fieldingexperiment.cpp \
        ../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/imageexperiment.cpp \
        ../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/readingexperiment.cpp \
        EyeTrackerInterface/HTCVIVEEyePro/calibrationtargets.cpp \
        EyeTrackerInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.cpp \
        EyeTrackerInterface/HTCVIVEEyePro/viveeyepoller.cpp \
        EyeTrackerInterface/eyetrackerdata.cpp \
        EyeTrackerInterface/eyetrackerinterface.cpp \
        control.cpp \
        main.cpp \
        qimagedisplay.cpp

RESOURCES += qml.qrc \
    expdata.qrc \
    images.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

QMAKE_CXXFLAGS_WARN_OFF = -Wfloat-equal



CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

## Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../../CommonClasses/CalibrationLeastSquares/calibrationleastsquares.h \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h \
    ../../../CommonClasses/Experiments/bindingmanager.h \
    ../../../CommonClasses/Experiments/bindingparser.h \
    ../../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../../CommonClasses/Experiments/fieldingmanager.h \
    ../../../CommonClasses/Experiments/readingmanager.h \
    ../../../CommonClasses/Experiments/readingparser.h \
    ../../../CommonClasses/LogInterface/loginterface.h \
    ../../../CommonClasses/OpenVRControlObject/openvr.h \
    ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h \
    ../../../CommonClasses/common.h \
    ../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/experiment.h \
    ../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/fieldingexperiment.h \
    ../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/imageexperiment.h \
    ../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/readingexperiment.h \
    EyeTrackerInterface/HTCVIVEEyePro/calibrationtargets.h \
    EyeTrackerInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h \
    EyeTrackerInterface/HTCVIVEEyePro/viveeyepoller.h \
    EyeTrackerInterface/eyetrackerdata.h \
    EyeTrackerInterface/eyetrackerinterface.h \
    control.h \
    qimagedisplay.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_Enums.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_Eye.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_EyeDataType.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_EyeData_v1.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_EyeData_v2.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_Eye_Calibration_Enums.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_Lip.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_LipData_v1.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_LipData_v2.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/ViveSR_Enums.h

LIBS += -L$$PWD/lib -lSRanipal
LIBS += -L$$PWD/../../../CommonClasses/OpenVRControlObject/libs -lopenvr_api
