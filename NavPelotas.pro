QT += core gui widgets
CONFIG += c++17 console

INCLUDEPATH += include fileModule

SOURCES += \
    src/main.cpp \
    src/Graph.cpp \
    src/trie.cpp \
    src/dijkstra.cpp \
    fileModule/fileModule.cpp

HEADERS += \
    include/Graph.h \
    include/trie.h \
    include/json.hpp \
    include/dijkstra.h \
    fileModule/fileModule.h

DISTFILES += \
    data/*.json \
    README.md \
    cppexamples.txt
