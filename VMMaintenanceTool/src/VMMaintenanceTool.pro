QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
   ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
   ../../CommonClasses/DirTools/dircompare.cpp \
   ../../CommonClasses/DirTools/dirrunner.cpp \
   ../../CommonClasses/RestAPIController/restapicontroller.cpp \
   debugoptions.cpp \
   langs.cpp \
   main.cpp \
   maintenancemanager.cpp \
   mainwindow.cpp \
   messagelogger.cpp \
   paths.cpp \
   supportcontact.cpp \
   supportdialog.cpp

HEADERS += \
   ../../CommonClasses/ConfigurationManager/configurationmanager.h \
   ../../CommonClasses/DirTools/dircompare.h \
   ../../CommonClasses/DirTools/dirrunner.h \
   ../../CommonClasses/RestAPIController/restapicontroller.h \
   debugoptions.h \
   defines.h \
   langs.h \
   maintenancemanager.h \
   mainwindow.h \
   messagelogger.h \
   paths.h \
   supportcontact.h \
   supportdialog.h


CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

FORMS += \
   mainwindow.ui \
   supportdialog.ui

RESOURCES += images.qrc \
   langs.qrc

RC_ICONS = vmwhite.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
