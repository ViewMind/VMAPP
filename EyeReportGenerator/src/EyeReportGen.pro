#-------------------------------------------------
#
# Project created by QtCreator 2018-02-04T15:51:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EyeReportGen
TEMPLATE = app

OBJECTS_DIR = OBJS
MOC_DIR     = MOCS

SOURCES += main.cpp\
        eyereportui.cpp \
    eyedataprocessingthread.cpp \
    EyeMatrixProcessor/eyematrixprocessor.cpp \
    EyeMatrixGenerator/edpbase.cpp \
    EyeMatrixGenerator/edpreading.cpp \
    EyeMatrixGenerator/movingwindowalgorithm.cpp \
    EyeMatrixGenerator/edpimages.cpp \
    EyeMatrixGenerator/edpfielding.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    EyeMatrixProcessor/dataset.cpp \
    EyeMatrixProcessor/csvcheckedreader.cpp \
    ../../CommonClasses/Experiments/bindingmanager.cpp \
    ../../CommonClasses/Experiments/experimentdatapainter.cpp \
    ../../CommonClasses/Experiments/fieldingmanager.cpp \
    ../../CommonClasses/Experiments/phrases.cpp \
    ../../CommonClasses/Experiments/readingmanager.cpp \
    imagereportdrawer.cpp \
    reportviewer.cpp

HEADERS  += eyereportui.h \
    eyedataprocessingthread.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    ../../CommonClasses/Experiments/common.h \
    EyeMatrixProcessor/eyematrixprocessor.h \
    EyeMatrixGenerator/edpbase.h \
    EyeMatrixGenerator/edpreading.h \
    EyeMatrixGenerator/movingwindowalgorithm.h \
    EyeMatrixGenerator/edpimages.h \
    EyeMatrixGenerator/edpfielding.h \
    EyeMatrixProcessor/dataset.h \
    EyeMatrixProcessor/csvcheckedreader.h \
    ../../CommonClasses/Experiments/bindingmanager.h \
    ../../CommonClasses/Experiments/common.h \
    ../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../CommonClasses/Experiments/fieldingmanager.h \
    ../../CommonClasses/Experiments/readingmanager.h \
    ../../CommonClasses/Experiments/phrases.h \
    imagereportdrawer.h \
    reportviewer.h


FORMS    += eyereportui.ui

RESOURCES += \
    resources.qrc
