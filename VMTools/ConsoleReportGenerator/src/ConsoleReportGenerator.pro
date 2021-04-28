QT     += core gui widgets

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
        ../../../CommonClasses/GraphicalReport/diagonosislogic.cpp \
        ../../../CommonClasses/GraphicalReport/imagereportdrawer.cpp \
        ../../../CommonClasses/GraphicalReport/resultsegment.cpp \
        main.cpp

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../../CommonClasses/GraphicalReport/diagonosislogic.h \
    ../../../CommonClasses/GraphicalReport/imagereportdrawer.h \
    ../../../CommonClasses/GraphicalReport/resultsegment.h \
    ../../../CommonClasses/common.h

RESOURCES += \
    report_res.qrc \
    test_report.qrc
