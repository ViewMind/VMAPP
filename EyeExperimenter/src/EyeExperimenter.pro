#-------------------------------------------------
#
# Project created by QtCreator 2017-07-21T17:40:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EyeExperimenter
TEMPLATE = app

MOC_DIR = MOCS
OBJECTS_DIR = OBJS

SOURCES += main.cpp\       
    EyeTrackerInterface/eyetrackerdata.cpp \
    EyeTrackerInterface/eyetrackerinterface.cpp \
    EyeTrackerInterface/Mouse/mouseinterface.cpp \
    EyeTrackerInterface/Mouse/calibrationarea.cpp \
    EyeTrackerInterface/RED/redinterface.cpp \
    Experiments/experiment.cpp \
    Experiments/readingexperiment.cpp \
    Experiments/imageexperiment.cpp \
    Experiments/fieldingexperiment.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/Experiments/fieldingmanager.cpp \
    ../../CommonClasses/Experiments/phrases.cpp \
    ../../CommonClasses/Experiments/readingmanager.cpp \
    ../../CommonClasses/Experiments/bindingmanager.cpp \
    ../../CommonClasses/Experiments/experimentdatapainter.cpp \
    maingui.cpp \
    experimentchooserdialog.cpp \
    eyetrackerselectiondialog.cpp \
    processorthread.cpp \
    monitorscreen.cpp \
    settingsdialog.cpp \
    instructiondialog.cpp


HEADERS  += maingui.h \
    EyeTrackerInterface/eyetrackerdata.h \
    EyeTrackerInterface/eyetrackerinterface.h \
    EyeTrackerInterface/Mouse/mouseinterface.h \
    EyeTrackerInterface/Mouse/calibrationarea.h \
    EyeTrackerInterface/RED/redinterface.h \
    EyeTrackerInterface/RED/iViewXAPI.h \
    Experiments/experiment.h \
    Experiments/readingexperiment.h \
    Experiments/imageexperiment.h \
    Experiments/fieldingexperiment.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/Experiments/fieldingmanager.h \
    ../../CommonClasses/Experiments/phrases.h \
    ../../CommonClasses/Experiments/readingmanager.h \
    ../../CommonClasses/Experiments/bindingmanager.h \
    ../../CommonClasses/Experiments/experimentdatapainter.h \
    processorthread.h \
    monitorscreen.h \
    experimentchooserdialog.h \
    eyetrackerselectiondialog.h \
    settingsdialog.h \
    instructiondialog.h


FORMS    += maingui.ui \
    experimentchooserdialog.ui \
    eyetrackerselectiondialog.ui \
    settingsdialog.ui

RESOURCES += \
    icons.qrc \
    images.qrc \
    expdata.qrc
win32: LIBS += -L$$PWD/EyeTrackerInterface/RED/ -liViewXAPI

INCLUDEPATH += $$PWD/EyeTrackerInterface/RED
DEPENDPATH += $$PWD/EyeTrackerInterface/RED
