QT += gui widgets
QT += core
CONFIG += c++17 console

INCLUDEPATH += include fileModule

SOURCES += src/main.cpp \
           src/Graph.cpp \
           src/trie.cpp \
           src/dijkstra.cpp \
           fileModule/fileModule.cpp

HEADERS += include/Graph.h \
           include/trie.h \
           include/json.hpp \
           include/dijkstra.h \
           fileModule/fileModule.h

# --- Skip copying JSONs on macOS to avoid errors ---
# unix {
#     copy_jsons.commands = mkdir -p $$OUT_PWD/data && cp -u $$PWD/data/*.json $$OUT_PWD/data
# }
# QMAKE_EXTRA_TARGETS += copy_jsons
# POST_TARGETDEPS += copy_jsons

# --- Keep data files visible in project tree ---
DISTFILES += data/*.json \
             data/*.docx \
             README.md \
             cppexamples.txt
