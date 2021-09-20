QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

SOURCES += \
        ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
        ../../../CommonClasses/DatFileInfo/datfileinfoindir.cpp \
        ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.cpp \
        ../../../CommonClasses/Experiments/fieldingparser.cpp \
        ../../../CommonClasses/Experiments/gonogoparser.cpp \
        ../../../CommonClasses/RawDataContainer/datfiletorawdatacontainer.cpp \
        ../../../CommonClasses/RawDataContainer/viewminddatacontainer.cpp \
        ../../../VMSoftwareSuite/EyeExperimenter/src/qualitycontrol.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../../CommonClasses/DatFileInfo/datfileinfoindir.h \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h \
    ../../../CommonClasses/Experiments/fieldingparser.h \
    ../../../CommonClasses/Experiments/gonogoparser.h \
    ../../../CommonClasses/RawDataContainer/VMDC.h \
    ../../../CommonClasses/RawDataContainer/datfiletorawdatacontainer.h \
    ../../../CommonClasses/RawDataContainer/viewminddatacontainer.h \
    ../../../CommonClasses/common.h \
    ../../../CommonClasses/eyetracker_defines.h \
    ../../../VMSoftwareSuite/EyeExperimenter/src/qualitycontrol.h
