QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
        ../../../CommonClasses/LogInterface/loggerthread.cpp \
        ../../../CommonClasses/LogInterface/staticthreadlogger.cpp \
        ../../../CommonClasses/RawDataContainer/viewminddatacontainer.cpp \
        ../../../EyeExperimenter/src/fuzzystringcompare.cpp \
        ../../../EyeExperimenter/src/localdb.cpp \
        ../../../EyeExperimenter/src/studyendoperations.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
   ../../../CommonClasses/ConfigurationManager/configurationmanager.h \
   ../../../CommonClasses/LogInterface/loggerthread.h \
   ../../../CommonClasses/LogInterface/staticthreadlogger.h \
   ../../../CommonClasses/RawDataContainer/VMDC.h \
   ../../../CommonClasses/RawDataContainer/viewminddatacontainer.h \
   ../../../CommonClasses/debug.h \
   ../../../CommonClasses/eyetracker_defines.h \
   ../../../EyeExperimenter/src/eyexperimenter_defines.h \
   ../../../EyeExperimenter/src/fuzzystringcompare.h \
   ../../../EyeExperimenter/src/localdb.h \
   ../../../EyeExperimenter/src/studyendoperations.h
