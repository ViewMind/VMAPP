#-------------------------------------------------
#
# Project created by QtCreator 2018-02-04T15:51:18
#
#-------------------------------------------------

QT       += core widgets gui

TARGET = EyeReportGen
TEMPLATE = app

OBJECTS_DIR = OBJS
MOC_DIR     = MOCS
RCC_DIR     = QRC
CONFIG     -= debug_and_release
CONFIG     += console

SOURCES += main.cpp\
    EyeMatrixProcessor/eyematrixprocessor.cpp \
    EyeMatrixGenerator/edpbase.cpp \
    EyeMatrixGenerator/edpreading.cpp \
    EyeMatrixGenerator/movingwindowalgorithm.cpp \
    EyeMatrixGenerator/edpimages.cpp \
    EyeMatrixGenerator/edpfielding.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    EyeMatrixProcessor/dataset.cpp \
    EyeMatrixProcessor/csvcheckedreader.cpp \
    ../../CommonClasses/Experiments/bindingmanager.cpp \
    ../../CommonClasses/Experiments/experimentdatapainter.cpp \
    ../../CommonClasses/Experiments/fieldingmanager.cpp \
    ../../CommonClasses/Experiments/phrases.cpp \
    ../../CommonClasses/Experiments/readingmanager.cpp \
    control.cpp \
    rawdataprocessor.cpp \
    htmlwriter.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp

HEADERS  += \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/common.h \
    EyeMatrixProcessor/eyematrixprocessor.h \
    EyeMatrixGenerator/edpbase.h \
    EyeMatrixGenerator/edpreading.h \
    EyeMatrixGenerator/movingwindowalgorithm.h \
    EyeMatrixGenerator/edpimages.h \
    EyeMatrixGenerator/edpfielding.h \
    EyeMatrixProcessor/dataset.h \
    EyeMatrixProcessor/csvcheckedreader.h \
    ../../CommonClasses/Experiments/bindingmanager.h \
    ../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../CommonClasses/Experiments/fieldingmanager.h \
    ../../CommonClasses/Experiments/readingmanager.h \
    ../../CommonClasses/Experiments/phrases.h \
    control.h \
    rawdataprocessor.h \
    htmlwriter.h \
    ../../CommonClasses/LogInterface/loginterface.h

RESOURCES +=
