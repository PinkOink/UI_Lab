TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/set_test.cpp

LIBS += -L$$PWD/libs/ -llogger -L$$PWD/libs/ -lvector -L$$PWD/libs/ -lset

HEADERS += \
    src/IVector.h \
    src/ILogger.h \
    src/RC.h \
    src/ISet.h
