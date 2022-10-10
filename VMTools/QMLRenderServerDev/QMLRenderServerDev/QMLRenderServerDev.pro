QT += quick network

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../CommonClasses/LogInterface/loginterface.cpp \
        ../../../CommonClasses/QMLQImageDisplay/qimagedisplay.cpp \
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
    ../../../CommonClasses/LogInterface/loginterface.h \
    ../../../CommonClasses/QMLQImageDisplay/qimagedisplay.h \
    ../../../CommonClasses/RenderServerClient/RenderServerPackets/RenderServerPacketNames.h \
    ../../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacket.h \
    ../../../CommonClasses/RenderServerClient/RenderServerPackets/renderserverpacketrecognizer.h \
    ../../../CommonClasses/RenderServerClient/renderserverclient.h \
    control.h
