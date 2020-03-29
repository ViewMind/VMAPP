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
    expdata.qrc \
    demo_data.qrc \
    report_text.qrc \
    agreements.qrc

SOURCES += main.cpp \
    ../../../CommonClasses/CalibrationLeastSquares/calibrationleastsquares.cpp \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.cpp \
    ../../../CommonClasses/Experiments/fieldingparser.cpp \
    ../../../CommonClasses/Experiments/parkinsonmanager.cpp \
    ../../../CommonClasses/Experiments/parkinsonparser.cpp \
    ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.cpp \
    ../../../CommonClasses/QMLQImageDisplay/qimagedisplay.cpp \
    Experiments/nbackrtexperiment.cpp \
    Experiments/parkinsonexperiment.cpp \
    EyeTrackerInterface/HTCVIVEEyePro/calibrationtargets.cpp \
    EyeTrackerInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.cpp \
    EyeTrackerInterface/HTCVIVEEyePro/viveeyepoller.cpp \
    loader.cpp \
    uiconfigmap.cpp \
    flowcontrol.cpp \
    monitorscreen.cpp \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../../CommonClasses/Experiments/bindingmanager.cpp \
    ../../../CommonClasses/Experiments/experimentdatapainter.cpp \
    ../../../CommonClasses/Experiments/fieldingmanager.cpp \
    ../../../CommonClasses/Experiments/readingmanager.cpp \
    ../../../CommonClasses/LogInterface/loginterface.cpp \
    ../../../CommonClasses/DataPacket/datapacket.cpp \
    ../../../CommonClasses/PNGWriter/imagereportdrawer.cpp \
    ../../../CommonClasses/Experiments/bindingparser.cpp \
    ../../../CommonClasses/DatFileInfo/datfileinfoindir.cpp \
    ../../../CommonClasses/PNGWriter/repfileinfo.cpp \
    ../../../CommonClasses/PNGWriter/resultbar.cpp \
    ../../../CommonClasses/LocalInformationManager/localinformationmanager.cpp \
    ../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.cpp \
    ../../../CommonClasses/Experiments/readingparser.cpp \
    ../../../CommonClasses/VariantMapSerializer/variantmapserializer.cpp \
    sslclient/ssldataprocessingclient.cpp \
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
    sslclient/sslclient.cpp


HEADERS += \
    ../../../CommonClasses/CalibrationLeastSquares/calibrationleastsquares.h \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h \
    ../../../CommonClasses/Experiments/fieldingparser.h \
    ../../../CommonClasses/Experiments/parkinsonmanager.h \
    ../../../CommonClasses/Experiments/parkinsonparser.h \
    ../../../CommonClasses/OpenVRControlObject/openvr.h \
    ../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h \
    ../../../CommonClasses/QMLQImageDisplay/qimagedisplay.h \
    Experiments/nbackrtexperiment.h \
    Experiments/parkinsonexperiment.h \
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
    loader.h \
    eye_experimenter_defines.h \
    flowcontrol.h \
    monitorscreen.h \
    countries.h \
    uiconfigmap.h \
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
    sslclient/sslclient.h \
    sslclient/ssldataprocessingclient.h \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../../CommonClasses/common.h \
    ../../../CommonClasses/common.h \
    ../../../CommonClasses/Experiments/bindingmanager.h \
    ../../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../../CommonClasses/Experiments/fieldingmanager.h \
    ../../../CommonClasses/Experiments/readingmanager.h \
    ../../../CommonClasses/common.h \
    ../../../CommonClasses/LogInterface/loginterface.h \
    ../../../CommonClasses/DataPacket/datapacket.h \
    ../../../CommonClasses/PNGWriter/imagereportdrawer.h \
    ../../../CommonClasses/SQLConn/dbdescription.h \
    ../../../CommonClasses/Experiments/bindingparser.h \
    ../../../CommonClasses/DatFileInfo/datfileinfoindir.h \
    ../../../CommonClasses/PNGWriter/repfileinfo.h \
    ../../../CommonClasses/PNGWriter/resultbar.h \
    ../../../CommonClasses/LocalInformationManager/localinformationmanager.h \
    ../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h \
    ../../../CommonClasses/Experiments/readingparser.h \
    ../../../CommonClasses/VariantMapSerializer/variantmapserializer.h

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



