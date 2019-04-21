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
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    control.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    ../../CommonClasses/DataAnalysis/rawdataprocessor.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpbase.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpfielding.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpimages.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpreading.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixProcessor/csvcheckedreader.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixProcessor/dataset.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixProcessor/eyematrixprocessor.cpp \
    ../../CommonClasses/HTMLWriter/htmlwriter.cpp \
    ../../CommonClasses/Experiments/bindingparser.cpp \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.cpp \
    ../../CommonClasses/DataAnalysis/BarGrapher/bargrapher.cpp

HEADERS  += \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/common.h \
    control.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    ../../CommonClasses/DataAnalysis/rawdataprocessor.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpbase.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpfielding.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpimages.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpreading.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixProcessor/csvcheckedreader.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixProcessor/dataset.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixProcessor/eyematrixprocessor.h \
    ../../CommonClasses/HTMLWriter/htmlwriter.h \
    ../../CommonClasses/SQLConn/dbdescription.h \
    ../../CommonClasses/Experiments/bindingparser.h \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.h \
    ../../CommonClasses/DataAnalysis/BarGrapher/bargrapher.h

RESOURCES += \
    bargraphs.qrc
