TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    systemtask.cpp \
    task.cpp \
    job.cpp

HEADERS += \
    systemtask.h \
    task.h \
    job.h

DISTFILES += \
    simDM \
    studyDM \
    taskGenerator \
    makefile \
    README.md


