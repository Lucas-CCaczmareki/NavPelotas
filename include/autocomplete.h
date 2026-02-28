#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

//caso windows
#ifdef _WIN32
#include <conio.h>
static int readKey() {
    int c = _getch();
    if (c == 0 || c == 0xE0) {
        int c2 = _getch();
        if (c2 == 72) return 0x141;
        if (c2 == 80) return 0x142;
        return -1;
    }
    return c;
}
static void enableRawMode()  {}
static void disableRawMode() {}
//caso linux/mac
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

static struct termios g_origTermios;
static bool g_rawModeActive = false;

static void disableRawMode() {
    if (!g_rawModeActive) return;
    tcsetattr(STDIN_FILENO, TCSANOW, &g_origTermios);
    std::cout << "\033[?25h" << std::flush;
    g_rawModeActive = false;
}

static void enableRawMode() {
    tcgetattr(STDIN_FILENO, &g_origTermios);
    struct termios raw = g_origTermios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    g_rawModeActive = true;
    atexit(disableRawMode);
}

static int readByteTimeout(int ms) {
    fd_set fds; FD_ZERO(&fds); FD_SET(STDIN_FILENO, &fds);
    struct timeval tv{ ms/1000, (ms%1000)*1000 };
    if (select(STDIN_FILENO+1, &fds, nullptr, nullptr, &tv) <= 0) return -1;
    unsigned char b;
    return (read(STDIN_FILENO, &b, 1) == 1) ? (int)b : -1;
}

static int readKey() {
    unsigned char c;
    while (true) {
        int n = read(STDIN_FILENO, &c, 1);
        if (n == 1) break;
        if (n < 0) return -1;
    }
    if (c == 0x1B) {
        int b1 = readByteTimeout(50); if (b1 == -1) return 0x1B;
        int b2 = readByteTimeout(50); if (b2 == -1) return 0x1B;
        if (b1 == '[') {
            if (b2 == 'A') return 0x141;
            if (b2 == 'B') return 0x142;
        }
        return -1;
    }
    return (int)c;
}
#endif

namespace term {
// apaga a linha inteira e vai para coluna 1
inline void eraseLine()   { std::cout << "\033[2K\033[1G"; }
// move N linhas acima (fica na coluna atual)
inline void moveUp(int n) { if (n > 0) std::cout << "\033[" << n << "A"; }
// move para a linha abaixo
inline void moveDown(int n) { if (n > 0) std::cout << "\033[" << n << "B"; }
inline void hideCursor()  { std::cout << "\033[?25l"; }
inline void showCursor()  { std::cout << "\033[?25h"; }
} // namespace term

struct AutocompleteResult {
    bool      cancelled = false;
    long long id        = -1;
    std::string label;
};

struct AutocompleteConfig {
    int maxVisible         = 8;
    std::string prompt         = ">> ";
    std::string colorSelected  = "\033[1;32m";
    std::string colorNormal    = "\033[0m";
    std::string colorHighlight = "\033[1;33m";
    std::string colorDim       = "\033[2m";
};

using SearchFn = std::vector<std::pair<std::string, long long>>
    (*)(const std::string& query, int maxResults);

// quantas linhas o bloco ocupa
static int linesOccupied(
    const AutocompleteConfig& cfg,
    const std::vector<std::pair<std::string, long long>>& sugg
    ) {
    int total = (int)sugg.size();
    if (total == 0) return 1;                          // só o prompt vazio
    if (total == 1) return 1;                          // prompt com resultado inline
    int lines = 1 + std::min(cfg.maxVisible, total);   // prompt + sugestões
    if (total > cfg.maxVisible) lines++;               // indicador de scroll
    return lines;
}

// sobe até a primeira linha do bloco,
//  apaga cada linha individualmente com \033[2K\033[1G,
//  imprime o conteúdo novo e move para baixo.
static void render(
    const AutocompleteConfig& cfg,
    const std::string& query,
    const std::vector<std::pair<std::string, long long>>& sugg,
    int selected,
    int scrollOffset,
    int prevLines
    ) {
    // Sobe até o início do bloco anterior
    if (prevLines > 1) term::moveUp(prevLines - 1);

    // Apaga e redesenha linha a linha
    int total      = (int)sugg.size();
    int newLines   = linesOccupied(cfg, sugg);

    // Linha do prompt
    term::eraseLine();
    if (total == 1) {
        // resultado único: inline no prompt
        std::cout << cfg.colorHighlight << cfg.prompt << cfg.colorNormal
                  << cfg.colorSelected  << sugg[0].first
                  << cfg.colorNormal;
    } else {
        std::cout << cfg.colorHighlight << cfg.prompt << cfg.colorNormal << query;
    }

    // Sugestões
    if (total > 1) {
        int visibleEnd = std::min(scrollOffset + cfg.maxVisible, total);
        for (int i = scrollOffset; i < visibleEnd; i++) {
            std::cout << "\n";
            term::eraseLine();
            if (i == selected)
                std::cout << cfg.colorSelected << "  > " << sugg[i].first << cfg.colorNormal;
            else
                std::cout << cfg.colorDim << "    " << sugg[i].first << cfg.colorNormal;
        }
        if (total > cfg.maxVisible) {
            std::cout << "\n";
            term::eraseLine();
            std::cout << cfg.colorDim << "    ... " << (selected+1) << "/" << total << cfg.colorNormal;
        }
    }

    // Se o bloco encolheu, apaga as linhas extras abaixo
    if (prevLines > newLines) {
        int extra = prevLines - newLines;
        for (int i = 0; i < extra; i++) {
            std::cout << "\n";
            term::eraseLine();
        }
        // volta para a última linha do novo bloco
        term::moveUp(extra);
    }

    std::cout.flush();
}

// principal
AutocompleteResult autocomplete(
    const std::string& /*promptMsg*/,
    SearchFn searchFn,
    const AutocompleteConfig& cfg = AutocompleteConfig{}
    ) {
    AutocompleteResult result;
    std::string query;
    std::vector<std::pair<std::string, long long>> sugg;
    int selected     = 0;
    int scrollOffset = 0;
    int prevLines    = 0;

    // reserva uma linha para o prompt inicial
    std::cout << "\n" << std::flush;

    enableRawMode();
    term::hideCursor();

    // posiciona na linha do prompt
    term::moveUp(1);
    render(cfg, query, sugg, selected, scrollOffset, prevLines);
    prevLines = linesOccupied(cfg, sugg);

    while (true) {
        int key = readKey();
        if (key == -1) continue;

        if (key == 0x1B || key == 27 || key == 3) {
            result.cancelled = true;
            break;
        }

        if (key == '\r' || key == '\n') {
            if (!sugg.empty()) {
                result.id    = sugg[selected].second;
                result.label = sugg[selected].first;
            }
            break;
        }

        if (key == '\b' || key == 127) {
            if (!query.empty()) {
                query.pop_back();
                selected = 0; scrollOffset = 0;
                sugg = searchFn(query, cfg.maxVisible * 4);
            }
            render(cfg, query, sugg, selected, scrollOffset, prevLines);
            prevLines = linesOccupied(cfg, sugg);
            continue;
        }

        if (key == 0x141) {
            if (selected > 0) { selected--; if (selected < scrollOffset) scrollOffset = selected; }
            render(cfg, query, sugg, selected, scrollOffset, prevLines);
            prevLines = linesOccupied(cfg, sugg);
            continue;
        }

        if (key == 0x142) {
            if (selected < (int)sugg.size() - 1) {
                selected++;
                if (selected >= scrollOffset + cfg.maxVisible)
                    scrollOffset = selected - cfg.maxVisible + 1;
            }
            render(cfg, query, sugg, selected, scrollOffset, prevLines);
            prevLines = linesOccupied(cfg, sugg);
            continue;
        }

        if (key >= 32 && key <= 126) {
            query += (char)key;
            selected = 0; scrollOffset = 0;
            sugg = searchFn(query, cfg.maxVisible * 4);
        }

        render(cfg, query, sugg, selected, scrollOffset, prevLines);
        prevLines = linesOccupied(cfg, sugg);
    }

    // apaga o bloco inteiro, deixa cursor na linha do prompt
    term::showCursor();
    disableRawMode();

    if (prevLines > 1) term::moveUp(prevLines - 1);
    for (int i = 0; i < prevLines; i++) {
        term::eraseLine();
        if (i < prevLines - 1) std::cout << "\n";
    }
    if (prevLines > 1) term::moveUp(prevLines - 1);

    std::cout.flush();

    return result;
}
