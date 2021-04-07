QT += widgets

CONFIG += c++17

SOURCES += main.cpp \
    nmlproject.cpp \
    spriteeditor.cpp \
    syntaxhighlighter.cpp \
    texteditor.cpp \
    texteditorlist.cpp

HEADERS += \
    nmlproject.h \
    spriteeditor.h \
    syntaxhighlighter.h \
    texteditor.h \
    texteditorlist.h \
    version.h \
    windowwithclosesignal.hpp

RESOURCES += \
    resource.qrc

win32:RC_FILE = resource.rc
