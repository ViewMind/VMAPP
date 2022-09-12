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
    ../../CommonClasses/Experiments/experiment_descriptions.qrc \
    fonts.qrc \
    images.qrc \
    languages.qrc

SOURCES += main.cpp \
    ../../CommonClasses/Experiments/binding/bindingexperiment.cpp \
    ../../CommonClasses/Experiments/binding/bindingmanager.cpp \
    ../../CommonClasses/Experiments/binding/bindingparser.cpp \
    ../../CommonClasses/Experiments/drawingconstantscalculator.cpp \
    ../../CommonClasses/Experiments/experiment.cpp \
    ../../CommonClasses/Experiments/experimentdatapainter.cpp \
    ../../CommonClasses/Experiments/gonogo/gonogoexperiment.cpp \
    ../../CommonClasses/Experiments/gonogo/gonogomanager.cpp \
    ../../CommonClasses/Experiments/gonogo/gonogoparser.cpp \
    ../../CommonClasses/Experiments/nbackfamiliy/nbackmanager.cpp \
    ../../CommonClasses/Experiments/nbackfamiliy/nbackparser.cpp \
    ../../CommonClasses/Experiments/nbackfamiliy/nbackrtexperiment.cpp \
    ../../CommonClasses/EyeSelector/eyeselector.cpp \
    ../../CommonClasses/EyeTrackingInterface/GazePoint/eventdetection.cpp \
    ../../CommonClasses/EyeTrackingInterface/GazePoint/opengazecommand.cpp \
    ../../CommonClasses/EyeTrackingInterface/GazePoint/opengazeinterface.cpp \
    ../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptinterface.cpp \
    ../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptprovider.cpp \
    ../../CommonClasses/EyeTrackingInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.cpp \
    ../../CommonClasses/EyeTrackingInterface/HTCVIVEEyePro/viveeyepoller.cpp \
    ../../CommonClasses/EyeTrackingInterface/Mouse/calibrationarea.cpp \
    ../../CommonClasses/EyeTrackingInterface/Mouse/mouseinterface.cpp \
    ../../CommonClasses/EyeTrackingInterface/calibrationleastsquares.cpp \
    ../../CommonClasses/EyeTrackingInterface/calibrationtargets.cpp \
    ../../CommonClasses/EyeTrackingInterface/eyecorrectioncoefficients.cpp \
    ../../CommonClasses/EyeTrackingInterface/eyerealdata.cpp \
    ../../CommonClasses/EyeTrackingInterface/eyetrackerdata.cpp \
    ../../CommonClasses/EyeTrackingInterface/eyetrackerinterface.cpp \
    ../../CommonClasses/FileDownloader/filedownloader.cpp \
    ../../CommonClasses/LinearLeastSquares/linearcoefficients.cpp \
    ../../CommonClasses/LinearLeastSquares/linearleastsquaresfit.cpp \
    ../../CommonClasses/MWA/fixation.cpp \
    ../../CommonClasses/MWA/movingwindowalgorithm.cpp \
    ../../CommonClasses/OpenVRControlObject/openvrcontrolobject.cpp \
    ../../CommonClasses/QMLQImageDisplay/qimagedisplay.cpp \
    ../../CommonClasses/RawDataContainer/viewminddatacontainer.cpp \
    ../../CommonClasses/RestAPIController/orbitpartnerapi.cpp \
    ../../CommonClasses/RestAPIController/partnerapi.cpp \
    ../../CommonClasses/RestAPIController/restapicontroller.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    apiclient.cpp \
    loader.cpp \
    flowcontrol.cpp \
    localdb.cpp \
    qualitycontrol.cpp \
    subjectdirscanner.cpp \
    updater.cpp


HEADERS += \
    ../../CommonClasses/Experiments/binding/bindingexperiment.h \
    ../../CommonClasses/Experiments/binding/bindingmanager.h \
    ../../CommonClasses/Experiments/binding/bindingparser.h \
    ../../CommonClasses/Experiments/drawingconstantscalculator.h \
    ../../CommonClasses/Experiments/experiment.h \
    ../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../CommonClasses/Experiments/gonogo/gonogoexperiment.h \
    ../../CommonClasses/Experiments/gonogo/gonogomanager.h \
    ../../CommonClasses/Experiments/gonogo/gonogoparser.h \
    ../../CommonClasses/Experiments/nbackfamiliy/nbackmanager.h \
    ../../CommonClasses/Experiments/nbackfamiliy/nbackparser.h \
    ../../CommonClasses/Experiments/nbackfamiliy/nbackrtexperiment.h \
    ../../CommonClasses/EyeSelector/eyeselector.h \
    ../../CommonClasses/EyeTrackingInterface/GazePoint/eventdetection.h \
    ../../CommonClasses/EyeTrackingInterface/GazePoint/opengazecommand.h \
    ../../CommonClasses/EyeTrackingInterface/GazePoint/opengazeinterface.h \
    ../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptinterface.h \
    ../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptprovider.h \
    ../../CommonClasses/EyeTrackingInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h \
    ../../CommonClasses/EyeTrackingInterface/HTCVIVEEyePro/viveeyepoller.h \
    ../../CommonClasses/EyeTrackingInterface/Mouse/calibrationarea.h \
    ../../CommonClasses/EyeTrackingInterface/Mouse/mouseinterface.h \
    ../../CommonClasses/EyeTrackingInterface/calibrationleastsquares.h \
    ../../CommonClasses/EyeTrackingInterface/calibrationtargets.h \
    ../../CommonClasses/EyeTrackingInterface/eyecorrectioncoefficients.h \
    ../../CommonClasses/EyeTrackingInterface/eyerealdata.h \
    ../../CommonClasses/EyeTrackingInterface/eyetrackerdata.h \
    ../../CommonClasses/EyeTrackingInterface/eyetrackerinterface.h \
    ../../CommonClasses/FileDownloader/filedownloader.h \
    ../../CommonClasses/LinearLeastSquares/linearcoefficients.h \
    ../../CommonClasses/LinearLeastSquares/linearleastsquaresfit.h \
    ../../CommonClasses/MWA/fixation.h \
    ../../CommonClasses/MWA/movingwindowalgorithm.h \
    ../../CommonClasses/OpenVRControlObject/openvr.h \
    ../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h \
    ../../CommonClasses/QMLQImageDisplay/qimagedisplay.h \
    ../../CommonClasses/RawDataContainer/VMDC.h \
    ../../CommonClasses/RawDataContainer/viewminddatacontainer.h \
    ../../CommonClasses/RestAPIController/orbitpartnerapi.h \
    ../../CommonClasses/RestAPIController/partnerapi.h \
    ../../CommonClasses/RestAPIController/restapicontroller.h \
    ../../CommonClasses/debug.h \
    ../../CommonClasses/eyetracker_defines.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    apiclient.h \
    eyexperimenter_defines.h \
    loader.h \
    flowcontrol.h \
    localdb.h \
    countries.h \
    qualitycontrol.h \
    subjectdirscanner.h \
    updater.h

#contains(DEFINES, USE_IVIEW) {
#  LIBS += -L$$PWD/EyeTrackerInterface/RED/ -liViewXAPI
#  INCLUDEPATH += $$PWD/EyeTrackerInterface/RED
#  DEPENDPATH += $$PWD/EyeTrackerInterface/RED
#  SOURCES += EyeTrackerInterface/RED/redinterface.cpp
#  HEADERS += EyeTrackerInterface/RED/iViewXAPI.h \
#             EyeTrackerInterface/RED/redinterface.h
#}

LIBS += -L$$PWD/../../CommonClasses/OpenVRControlObject/libs -lopenvr_api
LIBS += -L$$PWD/../../CommonClasses/EyeTrackingInterface/HPReverb/hp -lhp_omnicept
LIBS += -L$$PWD/../../CommonClasses/EyeTrackingInterface/HTCVIVEEyePro -lSRanipal
LIBS += -lWinTrust
LIBS += -lRpcRT4
LIBS += -lole32
LIBS += -lshell32
LIBS += -ladvapi32

DISTFILES += \
    ../../CommonClasses/Experiments/binding/descriptions/bc.dat \
    ../../CommonClasses/Experiments/binding/descriptions/bc_3.dat \
    ../../CommonClasses/Experiments/binding/descriptions/uc.dat \
    ../../CommonClasses/Experiments/binding/descriptions/uc_3.dat \
    ../../CommonClasses/Experiments/gonogo/descriptions/go_no_go.dat \
    ../../CommonClasses/Experiments/nbackfamiliy/descriptions/fielding.dat \
    ../../CommonClasses/Experiments/nbackfamiliy/descriptions/fielding_only3.dat \
    ../../CommonClasses/Experiments/parkinson/descriptions/parkinson.dat \
    ../../CommonClasses/Experiments/perception/descriptions/perception_study.dat \
    ../../CommonClasses/Experiments/reading/descriptions/Reading_de.dat \
    ../../CommonClasses/Experiments/reading/descriptions/Reading_de_original.dat \
    ../../CommonClasses/Experiments/reading/descriptions/Reading_en.dat \
    ../../CommonClasses/Experiments/reading/descriptions/Reading_es.dat \
    ../../CommonClasses/Experiments/reading/descriptions/Reading_fr.dat \
    ../../CommonClasses/Experiments/reading/descriptions/Reading_fr_original.dat \
    ../../CommonClasses/Experiments/reading/descriptions/Reading_is.dat \
    configs/config.cnf



