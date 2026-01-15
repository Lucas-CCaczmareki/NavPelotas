QT -= gui
QT += core
CONFIG += c++17 console
CONFIG -= app_bundle

INCLUDEPATH += include

# Adicione o fileModule aqui
SOURCES += src/main.cpp \
           src/Graph.cpp \
           src/trie.cpp \
           fileModule/fileModule.cpp

HEADERS += include/Graph.h \
           include/trie.h \
           include/json.hpp \
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

# --- Isso faz os arquivos aparecerem na árvore do projeto (apenas visual) ---
DISTFILES += \
    data/edges.json \
    data/nodes.json \
    data/label_to_nodes.json \
    data/nodes_to_label.json
