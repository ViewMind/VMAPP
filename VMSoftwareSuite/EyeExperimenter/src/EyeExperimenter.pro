QT += quick gui widgets network
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += USE_IVIEW

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

RESOURCES += qml.qrc \
    fonts.qrc \
    images.qrc \
    languages.qrc \
    expdata.qrc

SOURCES += main.cpp \
    ../../../CommonClasses/CalibrationLeastSquares/calibrationleastsquares.cpp \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.cpp \
    ../../../CommonClasses/Experiments/fieldingparser.cpp \
    ../../../CommonClasses/Experiments/gonogomanager.cpp \
    ../../../CommonClasses/Experiments/gonogoparser.cpp \
    ../../../CommonClasses/Experiments/parkinsonmanager.cpp \
    ../../../CommonClasses/Experiments/parkinsonparser.cpp \
    ../../../CommonClasses/Experiments/perceptionmanager.cpp \
    ../../../CommonClasses/Experiments/perceptionparser.cpp \
    ../../../CommonClasses/EyeSelector/eyeselector.cpp \
    ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.cpp \
    ../../../CommonClasses/QMLQImageDisplay/qimagedisplay.cpp \
    ../../../CommonClasses/RawDataContainer/viewminddatacontainer.cpp \
    ../../../CommonClasses/RestAPIController/restapicontroller.cpp \
    Experiments/gonogoexperiment.cpp \
    Experiments/nbackrtexperiment.cpp \
    Experiments/parkinsonexperiment.cpp \
    Experiments/perceptionexperiment.cpp \
    EyeTrackerInterface/HTCVIVEEyePro/calibrationtargets.cpp \
    EyeTrackerInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.cpp \
    EyeTrackerInterface/HTCVIVEEyePro/viveeyepoller.cpp \
    apiclient.cpp \
    loader.cpp \
    flowcontrol.cpp \
    localdb.cpp \
    monitorscreen.cpp \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../../CommonClasses/Experiments/bindingmanager.cpp \
    ../../../CommonClasses/Experiments/experimentdatapainter.cpp \
    ../../../CommonClasses/Experiments/fieldingmanager.cpp \
    ../../../CommonClasses/Experiments/readingmanager.cpp \
    ../../../CommonClasses/LogInterface/loginterface.cpp \
    ../../../CommonClasses/Experiments/bindingparser.cpp \
    ../../../CommonClasses/Experiments/readingparser.cpp \
    EyeTrackerInterface/GazePoint/eventdetection.cpp \
    EyeTrackerInterface/GazePoint/opengazecommand.cpp \
    EyeTrackerInterface/GazePoint/opengazeinterface.cpp \
    EyeTrackerInterface/eyetrackerdata.cpp \
    EyeTrackerInterface/eyetrackerinterface.cpp \
    EyeTrackerInterface/Mouse/calibrationarea.cpp \
    EyeTrackerInterface/Mouse/mouseinterface.cpp \
    Experiments/experiment.cpp \
    Experiments/fieldingexperiment.cpp \
    Experiments/imageexperiment.cpp \
    Experiments/readingexperiment.cpp \
    qualitycontrol.cpp \
    subjectdirscanner.cpp


HEADERS += \
    ../../../CommonClasses/CalibrationLeastSquares/calibrationleastsquares.h \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h \
    ../../../CommonClasses/Experiments/fieldingparser.h \
    ../../../CommonClasses/Experiments/gonogomanager.h \
    ../../../CommonClasses/Experiments/gonogoparser.h \
    ../../../CommonClasses/Experiments/parkinsonmanager.h \
    ../../../CommonClasses/Experiments/parkinsonparser.h \
    ../../../CommonClasses/Experiments/perceptionmanager.h \
    ../../../CommonClasses/Experiments/perceptionparser.h \
    ../../../CommonClasses/EyeSelector/eyeselector.h \
    ../../../CommonClasses/OpenVRControlObject/openvr.h \
    ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h \
    ../../../CommonClasses/QMLQImageDisplay/qimagedisplay.h \
    ../../../CommonClasses/RawDataContainer/VMDC.h \
    ../../../CommonClasses/RawDataContainer/viewminddatacontainer.h \
    ../../../CommonClasses/RestAPIController/restapicontroller.h \
    ../../../CommonClasses/debug.h \
    ../../../CommonClasses/eyetracker_defines.h \
    Experiments/gonogoexperiment.h \
    Experiments/nbackrtexperiment.h \
    Experiments/parkinsonexperiment.h \
    Experiments/perceptionexperiment.h \
    EyeTrackerInterface/HTCVIVEEyePro/calibrationtargets.h \
    EyeTrackerInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_Enums.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_Eye.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_EyeDataType.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_EyeData_v1.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_EyeData_v2.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_Eye_Calibration_Enums.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_Lip.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_LipData_v1.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/SRanipal_LipData_v2.h \
    EyeTrackerInterface/HTCVIVEEyePro/sranipal/ViveSR_Enums.h \
    EyeTrackerInterface/HTCVIVEEyePro/viveeyepoller.h \
    apiclient.h \
    eyexperimenter_defines.h \
    loader.h \
    flowcontrol.h \
    localdb.h \
    monitorscreen.h \
    countries.h \
    EyeTrackerInterface/eyetrackerdata.h \
    EyeTrackerInterface/eyetrackerinterface.h \
    EyeTrackerInterface/Mouse/calibrationarea.h \
    EyeTrackerInterface/Mouse/mouseinterface.h \
    EyeTrackerInterface/GazePoint/eventdetection.h \
    EyeTrackerInterface/GazePoint/opengazecommand.h \
    EyeTrackerInterface/GazePoint/opengazeinterface.h \
    Experiments/experiment.h \
    Experiments/fieldingexperiment.h \
    Experiments/imageexperiment.h \
    Experiments/readingexperiment.h \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../../CommonClasses/Experiments/bindingmanager.h \
    ../../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../../CommonClasses/Experiments/fieldingmanager.h \
    ../../../CommonClasses/Experiments/readingmanager.h \
    ../../../CommonClasses/LogInterface/loginterface.h \
    ../../../CommonClasses/Experiments/bindingparser.h \
    ../../../CommonClasses/Experiments/readingparser.h \
    qualitycontrol.h \
    subjectdirscanner.h

contains(DEFINES, USE_IVIEW) {
  LIBS += -L$$PWD/EyeTrackerInterface/RED/ -liViewXAPI
  INCLUDEPATH += $$PWD/EyeTrackerInterface/RED
  DEPENDPATH += $$PWD/EyeTrackerInterface/RED
  SOURCES += EyeTrackerInterface/RED/redinterface.cpp
  HEADERS += EyeTrackerInterface/RED/iViewXAPI.h \
             EyeTrackerInterface/RED/redinterface.h

}

LIBS += -L$$PWD/lib -lSRanipal
LIBS += -L$$PWD/../../../CommonClasses/OpenVRControlObject/libs -lopenvr_api



