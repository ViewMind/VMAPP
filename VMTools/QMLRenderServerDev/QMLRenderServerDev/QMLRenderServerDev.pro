QT += quick network

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
        ../../../CommonClasses/Experiments/binding/bindingmanager.cpp \
        ../../../CommonClasses/Experiments/binding/bindingparser.cpp \
        ../../../CommonClasses/Experiments/drawingconstantscalculator.cpp \
        ../../../CommonClasses/Experiments/experimentdatapainter.cpp \
        ../../../CommonClasses/Experiments/gonogo/gonogomanager.cpp \
        ../../../CommonClasses/Experiments/gonogo/gonogoparser.cpp \
        ../../../CommonClasses/Experiments/nbackfamiliy/nbackmanager.cpp \
        ../../../CommonClasses/Experiments/nbackfamiliy/nbackparser.cpp \
        ../../../CommonClasses/Experiments/linemath.cpp \
        ../../../CommonClasses/LogInterface/loggerthread.cpp \
        ../../../CommonClasses/LogInterface/staticthreadlogger.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/animationmanager.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverarrowitem.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderservercircleitem.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverimageitem.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserveritem.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserveritemgroup.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverlineitem.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverrectitem.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverscene.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderservertextitem.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderservertriangleitem.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacket.cpp \
        ../../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacketrecognizer.cpp \
        ../../../CommonClasses/RenderServerClient/renderserverclient.cpp \
        control.cpp \        
        main.cpp

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../../CommonClasses/Experiments/binding/bindingmanager.h \
    ../../../CommonClasses/Experiments/binding/bindingparser.h \
    ../../../CommonClasses/Experiments/drawingconstantscalculator.h \
    ../../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../../CommonClasses/Experiments/gonogo/gonogomanager.h \
    ../../../CommonClasses/Experiments/gonogo/gonogoparser.h \
    ../../../CommonClasses/Experiments/nbackfamiliy/nbackmanager.h \
    ../../../CommonClasses/Experiments/nbackfamiliy/nbackparser.h \
    ../../../CommonClasses/Experiments/linemath.h \
    ../../../CommonClasses/LogInterface/loggerthread.h \
    ../../../CommonClasses/LogInterface/staticthreadlogger.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/animationmanager.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverarrowitem.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderservercircleitem.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverimageitem.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserveritem.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserveritemgroup.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverlineitem.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverrectitem.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderserverscene.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderservertextitem.h \
    ../../../CommonClasses/RenderServerClient/RenderServerGraphics/renderservertriangleitem.h \
    ../../../CommonClasses/RenderServerClient/RenderServerPackets/RenderServerPacketNames.h \
    ../../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacket.h \
    ../../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacketrecognizer.h \
    ../../../CommonClasses/RenderServerClient/renderserverclient.h \
    ../../../CommonClasses/debug.h \
    ../../../CommonClasses/eyetracker_defines.h \
    control.h

LIBS += -lWinTrust
LIBS += -lRpcRT4
LIBS += -lole32
LIBS += -lshell32
LIBS += -ladvapi32
LIBS += -luser32
