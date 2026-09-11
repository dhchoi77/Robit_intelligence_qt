QT       += core gui widgets

CONFIG   += c++17
TARGET    = keyboard
TEMPLATE  = app

SOURCES += \
    main.cpp \
    KeyboardWidget.cpp \
    KeyButton.cpp \
    HangulComposer.cpp

HEADERS += \
    KeyboardWidget.h \
    KeyButton.h \
    HangulComposer.h
