QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

SOURCES += \
        ../../../CommonClasses/CalibrationLeastSquares/calibrationleastsquares.cpp \
        main.cpp


HEADERS += \
    ../../../CommonClasses/CalibrationLeastSquares/calibrationleastsquares.h
