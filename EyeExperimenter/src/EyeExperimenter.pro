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
    fonts.qrc \
    images.qrc \
    languages.qrc \
    expdata.qrc \
    demo_data.qrc

SOURCES += main.cpp \
    loader.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/Experiments/bindingmanager.cpp \
    ../../CommonClasses/Experiments/experimentdatapainter.cpp \
    ../../CommonClasses/Experiments/fieldingmanager.cpp \
    ../../CommonClasses/Experiments/phrases.cpp \
    ../../CommonClasses/Experiments/readingmanager.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    EyeTrackerInterface/eyetrackerdata.cpp \
    EyeTrackerInterface/eyetrackerinterface.cpp \
    EyeTrackerInterface/Mouse/calibrationarea.cpp \
    EyeTrackerInterface/Mouse/mouseinterface.cpp \
    flowcontrol.cpp \
    Experiments/experiment.cpp \
    Experiments/fieldingexperiment.cpp \
    Experiments/imageexperiment.cpp \
    Experiments/readingexperiment.cpp \
    monitorscreen.cpp \
    EyeTrackerInterface/RED/redinterface.cpp \
    ../../CommonClasses/DataPacket/datapacket.cpp \
    sslclient/sslclient.cpp

HEADERS += \
    loader.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/Experiments/bindingmanager.h \
    ../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../CommonClasses/Experiments/fieldingmanager.h \
    ../../CommonClasses/Experiments/phrases.h \
    ../../CommonClasses/Experiments/readingmanager.h \
    eye_experimenter_defines.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    EyeTrackerInterface/eyetrackerdata.h \
    EyeTrackerInterface/eyetrackerinterface.h \
    EyeTrackerInterface/Mouse/calibrationarea.h \
    EyeTrackerInterface/Mouse/mouseinterface.h \
    flowcontrol.h \
    Experiments/experiment.h \
    Experiments/fieldingexperiment.h \
    Experiments/imageexperiment.h \
    Experiments/readingexperiment.h \
    monitorscreen.h \
    EyeTrackerInterface/RED/iViewXAPI.h \
    EyeTrackerInterface/RED/redinterface.h \
    ../../CommonClasses/DataPacket/datapacket.h \
    sslclient/sslclient.h

win32: LIBS += -L$$PWD/EyeTrackerInterface/RED/ -liViewXAPI

INCLUDEPATH += $$PWD/EyeTrackerInterface/RED
DEPENDPATH += $$PWD/EyeTrackerInterface/RED

FORMS +=

