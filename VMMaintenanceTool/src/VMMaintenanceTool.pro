QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
   ../../CommonClasses/DirTools/dircompare.cpp \
   ../../CommonClasses/DirTools/dirrunner.cpp \
   main.cpp \
   mainwindow.cpp \
   messagelogger.cpp

HEADERS += \
   ../../CommonClasses/DirTools/dircompare.h \
   ../../CommonClasses/DirTools/dirrunner.h \
   defines.h \
   mainwindow.h \
   messagelogger.h


CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

FORMS += \
   mainwindow.ui

RESOURCES += images.qrc

RC_ICONS = vmwhite.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
