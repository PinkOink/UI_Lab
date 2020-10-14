TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/vector_test.cpp

LIBS += -L$$PWD/libs/ -llogger -L$$PWD/libs/ -lvector

HEADERS += \
    src/IVector.h \
    src/ILogger.h \
    src/RC.h
