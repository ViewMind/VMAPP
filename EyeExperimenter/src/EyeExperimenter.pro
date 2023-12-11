QT += quick gui widgets network
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
    dbugdata.qrc \
    fonts.qrc \
    images.qrc \
    languages.qrc \
    res.qrc \
    study_files.qrc

SOURCES += main.cpp \
    ../../CommonClasses/BallPathGenerator/ballpathgenerator.cpp \
    ../../CommonClasses/Calibration/calibrationhistory.cpp \
    ../../CommonClasses/Calibration/calibrationmanager.cpp \
    ../../CommonClasses/Calibration/calibrationvalidation.cpp \
    ../../CommonClasses/Calibration/eyecorrectioncoefficients.cpp \
    ../../CommonClasses/EyeTrackingInterface/eyetrackerdata.cpp \
    ../../CommonClasses/FileDownloader/filedownloader.cpp \
    ../../CommonClasses/HWRecog/hwrecognizer.cpp \
    ../../CommonClasses/HWRecog/tableoutputparser.cpp \
    ../../CommonClasses/LinearLeastSquares/linearcoefficients.cpp \
    ../../CommonClasses/LinearLeastSquares/linearleastsquaresfit.cpp \
    ../../CommonClasses/LinearLeastSquares/ordinaryleastsquares.cpp \
    ../../CommonClasses/LinearLeastSquares/simplematrix.cpp \
    ../../CommonClasses/LogInterface/loggerthread.cpp \
    ../../CommonClasses/LogInterface/logprep.cpp \
    ../../CommonClasses/LogInterface/staticthreadlogger.cpp \
    ../../CommonClasses/RawDataContainer/viewminddatacontainer.cpp \
    ../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacket.cpp \
    ../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacketrecognizer.cpp \
    ../../CommonClasses/RenderServerClient/renderserverclient.cpp \
    ../../CommonClasses/RestAPIController/restapicontroller.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/StudyControl/binding/bindingconfigurator.cpp \
    ../../CommonClasses/StudyControl/gng/gngconfigurator.cpp \
    ../../CommonClasses/StudyControl/gng3D/gngspheresconfigurator.cpp \
    ../../CommonClasses/StudyControl/nback/nbackconfigurator.cpp \
    ../../CommonClasses/StudyControl/studyconfigurator.cpp \
    ../../CommonClasses/StudyControl/studycontrol.cpp \
    apiclient.cpp \
    fuzzystringcompare.cpp \
    loader.cpp \
    flowcontrol.cpp \
    localdb.cpp \
    studyendoperations.cpp


HEADERS += \
    ../../CommonClasses/BallPathGenerator/ballpathgenerator.h \
    ../../CommonClasses/Calibration/calibrationhistory.h \
    ../../CommonClasses/Calibration/calibrationmanager.h \
    ../../CommonClasses/Calibration/calibrationvalidation.h \
    ../../CommonClasses/Calibration/eyecorrectioncoefficients.h \
    ../../CommonClasses/EyeTrackingInterface/eyetrackerdata.h \
    ../../CommonClasses/FileDownloader/filedownloader.h \
    ../../CommonClasses/HWRecog/hwrecognizer.h \
    ../../CommonClasses/HWRecog/tableoutputparser.h \
    ../../CommonClasses/LinearLeastSquares/linearcoefficients.h \
    ../../CommonClasses/LinearLeastSquares/linearleastsquaresfit.h \
    ../../CommonClasses/LinearLeastSquares/ordinaryleastsquares.h \
    ../../CommonClasses/LinearLeastSquares/simplematrix.h \
    ../../CommonClasses/LogInterface/loggerthread.h \
    ../../CommonClasses/LogInterface/logprep.h \
    ../../CommonClasses/LogInterface/staticthreadlogger.h \
    ../../CommonClasses/RawDataContainer/VMDC.h \
    ../../CommonClasses/RawDataContainer/viewminddatacontainer.h \
    ../../CommonClasses/RenderServerClient/RenderServerPackets/RenderServerStrings.h \
    ../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacket.h \
    ../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacketrecognizer.h \
    ../../CommonClasses/RenderServerClient/renderserverclient.h \
    ../../CommonClasses/RestAPIController/restapicontroller.h \
    ../../CommonClasses/StudyControl/StudyConfiguration.h \
    ../../CommonClasses/StudyControl/binding/bindingconfigurator.h \
    ../../CommonClasses/StudyControl/gng/gngconfigurator.h \
    ../../CommonClasses/StudyControl/gng3D/gngspheresconfigurator.h \
    ../../CommonClasses/StudyControl/nback/nbackconfigurator.h \
    ../../CommonClasses/StudyControl/studyconfigurator.h \
    ../../CommonClasses/StudyControl/studycontrol.h \
    ../../CommonClasses/debug.h \
    ../../CommonClasses/debug.h \
    ../../CommonClasses/eyetracker_defines.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    apiclient.h \
    eyexperimenter_defines.h \
    fuzzystringcompare.h \
    loader.h \
    flowcontrol.h \
    localdb.h \
    studyendoperations.h

RC_ICONS = vm.ico

LIBS += -lWinTrust
LIBS += -lRpcRT4
LIBS += -lole32
LIBS += -lshell32
LIBS += -ladvapi32
LIBS += -luser32

#DISTFILES += \
#    ../../CommonClasses/Experiments/binding/descriptions/bc.dat \
#    ../../CommonClasses/Experiments/binding/descriptions/bc_3.dat \
#    ../../CommonClasses/Experiments/binding/descriptions/uc.dat \
#    ../../CommonClasses/Experiments/binding/descriptions/uc_3.dat \
#    ../../CommonClasses/Experiments/gonogo/descriptions/go_no_go.dat \
#    ../../CommonClasses/Experiments/nbackfamiliy/descriptions/fielding.dat \
#    ../../CommonClasses/Experiments/nbackfamiliy/descriptions/fielding_only3.dat \
#    ../../CommonClasses/Experiments/parkinson/descriptions/parkinson.dat \
#    ../../CommonClasses/Experiments/perception/descriptions/perception_study.dat \
#    ../../CommonClasses/Experiments/reading/descriptions/Reading_de.dat \
#    ../../CommonClasses/Experiments/reading/descriptions/Reading_de_original.dat \
#    ../../CommonClasses/Experiments/reading/descriptions/Reading_en.dat \
#    ../../CommonClasses/Experiments/reading/descriptions/Reading_es.dat \
#    ../../CommonClasses/Experiments/reading/descriptions/Reading_fr.dat \
#    ../../CommonClasses/Experiments/reading/descriptions/Reading_fr_original.dat \
#    ../../CommonClasses/Experiments/reading/descriptions/Reading_is.dat \
#    configs/config.cnf



