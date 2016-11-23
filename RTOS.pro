TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    systemtask.cpp \
    task.cpp \
    scheduler.cpp \
    easyBMP/EasyBMP.cpp \
    easyBMP/EasyBMP_Font.cpp \
    easyBMP/EasyBMP_Geometry.cpp

HEADERS += \
    systemtask.h \
    task.h \
    scheduler.h \
    easyBMP/EasyBMP.h \
    easyBMP/EasyBMP_BMP.h \
    easyBMP/EasyBMP_DataStructures.h \
    easyBMP/EasyBMP_VariousBMPutilities.h \
    easyBMP/EasyBMP_Font.h \
    easyBMP/EasyBMP_Geometry.h

DISTFILES += \
    makefile \
    README.md


