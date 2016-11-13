TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    systemtask.cpp \
    task.cpp \
    job.cpp \
    scheduler.cpp

HEADERS += \
    systemtask.h \
    task.h \
    job.h \
    scheduler.h

DISTFILES += \
    makefile \
    README.md


