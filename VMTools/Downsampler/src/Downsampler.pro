QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

SOURCES += \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../../CommonClasses/DataAnalysis/DataSetExtractor/datasetextractor.cpp \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpbase.cpp \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpfielding.cpp \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpimages.cpp \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpnbackrt.cpp \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpreading.cpp \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.cpp \
    ../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.cpp \
    ../../../CommonClasses/Experiments/bindingparser.cpp \
    ../../../CommonClasses/Experiments/fieldingparser.cpp \
    ../../../CommonClasses/Experiments/parkinsonparser.cpp \
    ../../../CommonClasses/Experiments/readingparser.cpp \
    ../../../CommonClasses/LogInterface/loginterface.cpp \
    datasetdownsampler.cpp \
    frequencyanddispersionsweeper.cpp \
    main.cpp \
    eyedatadownsampler.cpp

HEADERS += \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../../CommonClasses/DataAnalysis/DataSetExtractor/datasetextractor.h \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpbase.h \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpfielding.h \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpimages.h \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpnbackrt.h \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpreading.h \
    ../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h \
    ../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h \
    ../../../CommonClasses/Experiments/bindingparser.h \
    ../../../CommonClasses/Experiments/fieldingparser.h \
    ../../../CommonClasses/Experiments/parkinsonparser.h \
    ../../../CommonClasses/Experiments/readingparser.h \
    ../../../CommonClasses/LogInterface/loginterface.h \
    ../../../CommonClasses/common.h \
    datasetdownsampler.h \
    eyedatadownsampler.h \
    frequencyanddispersionsweeper.h

FORMS += \
    eyedatadownsampler.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
