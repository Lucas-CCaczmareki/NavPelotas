QT += gui widgets
QT += core
CONFIG += c++17 console


INCLUDEPATH += include

# Adicione o fileModule aqui
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

# Cópia dos JSONs (Mantenha isso, é vital)
COPY_FILES += $$PWD/data/*.json
win32 {
    copy_files.commands = $(COPY_DIR) \"$$shell_path($$PWD/data)\" \"$$shell_path($$OUT_PWD/data)\"
}
unix {
    copy_files.commands = mkdir -p $$OUT_PWD/data && cp $$PWD/data/*.json $$OUT_PWD/data
}
QMAKE_EXTRA_TARGETS += copy_files
POST_TARGETDEPS += copy_files

INCLUDEPATH += include fileModule
# --- Isso faz os arquivos aparecerem na árvore do projeto (apenas visual) ---
DISTFILES += \
# Isso faz TODOS os arquivos da pasta data aparecerem na árvore do projeto
DISTFILES += \
    data/*.json \
    data/*.docx \
    README.md \
    cppexamples.txt
