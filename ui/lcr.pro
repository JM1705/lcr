TEMPLATE = app
TARGET = "Light Control Remapper"

QT = core gui

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

SOURCES += \
    main.cpp \
    maintoolbar.cpp \
    window.cpp

HEADERS += \
    maintoolbar.h \
    window.h
