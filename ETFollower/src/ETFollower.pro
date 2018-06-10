#-------------------------------------------------
#
# Project created by QtCreator 2018-06-09T05:30:51
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ETFollower
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
        etfollower.cpp \
    ../../EyeExperimenter/src/EyeTrackerInterface/eyetrackerdata.cpp \
    ../../EyeExperimenter/src/EyeTrackerInterface/eyetrackerinterface.cpp \
    ../../EyeExperimenter/src/EyeTrackerInterface/Mouse/calibrationarea.cpp \
    ../../EyeExperimenter/src/EyeTrackerInterface/Mouse/mouseinterface.cpp \
    followscreen.cpp \
    ../../EyeExperimenter/src/EyeTrackerInterface/GazePoint/opengazeinterface.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    ../../EyeExperimenter/src/EyeTrackerInterface/GazePoint/opengazecommand.cpp \
    ../../EyeExperimenter/src/EyeTrackerInterface/GazePoint/eventdetection.cpp

HEADERS += \
        etfollower.h \
    ../../EyeExperimenter/src/EyeTrackerInterface/eyetrackerdata.h \
    ../../EyeExperimenter/src/EyeTrackerInterface/eyetrackerinterface.h \
    ../../EyeExperimenter/src/EyeTrackerInterface/Mouse/calibrationarea.h \
    ../../EyeExperimenter/src/EyeTrackerInterface/Mouse/mouseinterface.h \
    followscreen.h \
    ../../EyeExperimenter/src/EyeTrackerInterface/GazePoint/opengazeinterface.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    ../../EyeExperimenter/src/EyeTrackerInterface/GazePoint/opengazecommand.h \
    ../../EyeExperimenter/src/EyeTrackerInterface/GazePoint/eventdetection.h

FORMS += \
        etfollower.ui
