#-------------------------------------------------
#
# Project created by QtCreator 2018-06-24T17:09:30
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EyeDataAnalyzer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

SOURCES += \
        main.cpp \
        eyedataanalyzer.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    ../../CommonClasses/DataAnalysis/rawdataprocessor.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpbase.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpfielding.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpimages.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpreading.cpp \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.cpp \
    fixationdrawer.cpp \
    ../../CommonClasses/Experiments/bindingmanager.cpp \
    ../../CommonClasses/Experiments/experimentdatapainter.cpp \
    ../../CommonClasses/Experiments/fieldingmanager.cpp \
    ../../CommonClasses/Experiments/readingmanager.cpp \
    ../../CommonClasses/HTMLWriter/htmlwriter.cpp \
    ../../CommonClasses/PNGWriter/imagereportdrawer.cpp \
    ../../CommonClasses/Experiments/bindingparser.cpp \
    ../../CommonClasses/PNGWriter/repfileinfo.cpp \
    ../../CommonClasses/PNGWriter/resultbar.cpp \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.cpp \
    ../../CommonClasses/Experiments/readingparser.cpp \
    ../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.cpp \
    ../../CommonClasses/DataPacket/datapacket.cpp \
    waitdialog.cpp \
    ../../CommonClasses/DataAnalysis/BarGrapher/bargrapher.cpp \
    patientnamemapmanager.cpp \
    ../../CommonClasses/DataAnalysis/RScriptCaller/rdataprocessor.cpp \
    ../../CommonClasses/VariantMapSerializer/variantmapserializer.cpp \
    batchcsvprocessing.cpp

HEADERS += \
        eyedataanalyzer.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    ../../CommonClasses/DataAnalysis/rawdataprocessor.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpbase.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpfielding.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpimages.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpreading.h \
    ../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h \
    fixationdrawer.h \
    ../../CommonClasses/Experiments/bindingmanager.h \
    ../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../CommonClasses/Experiments/fieldingmanager.h \
    ../../CommonClasses/Experiments/readingmanager.h \
    ../../CommonClasses/HTMLWriter/htmlwriter.h \
    ../../CommonClasses/PNGWriter/imagereportdrawer.h \
    ../../CommonClasses/Experiments/bindingparser.h \
    ../../CommonClasses/PNGWriter/repfileinfo.h \
    ../../CommonClasses/PNGWriter/resultbar.h \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.h \
    ../../CommonClasses/Experiments/readingparser.h \
    ../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h \
    ../../CommonClasses/DataPacket/datapacket.h \
    waitdialog.h \
    ../../CommonClasses/DataAnalysis/BarGrapher/bargrapher.h \
    patientnamemapmanager.h \
    ../../CommonClasses/DataAnalysis/RScriptCaller/rdataprocessor.h \
    ../../CommonClasses/VariantMapSerializer/variantmapserializer.h \
    batchcsvprocessing.h

FORMS += \
        eyedataanalyzer.ui \
    waitdialog.ui

RESOURCES += \
    report_res.qrc \
    bargrapher.qrc
