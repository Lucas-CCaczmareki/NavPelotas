#include <iostream>
#include <vector>
#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#endif
#include <string>
#include <filesystem>
#include <unordered_map>
#include <cmath>
#include "Graph.h"
#include "trie.h"
#include "./../fileModule/fileModule.h"
#include "dijkstra.h"
#include "autocomplete.h"
//#include <limits.h>

// mapa para busca reversa (id -> nome da rua)
std::unordered_map<unsigned long long, std::string> idToLabelMap;

// Ponteiro global para a trie — necessário porque SearchFn não captura estado
static trie::TrieNode* g_trieRoot = nullptr;

// callback para popular a trie e o mapa reverso simultaneamente
// chamada pelo filemodule
void loadDataCallback(trie::TrieNode* root, std::string label, unsigned long long id) {
    trie::insertWord(root, label, id);
    idToLabelMap[id] = label;
}


// imprime a rota agrupando ruas com mesmo nome
void printDetailedRoute(Graph* g, const std::vector<Dijkstra::Prev>& path, int destIdx) {
    std::cout << "\n================ ROTA DETALHADA ================\n";

    std::string lastStreet = "";
    int step = 0;

    for (const auto& p : path) {
        long long osmId = g->getIdFromIndex(p.node);
        std::string currentStreet = "Rua desconhecida (ID " + std::to_string(osmId) + ")";

        if (idToLabelMap.count(osmId)) {
            currentStreet = idToLabelMap[osmId];
        }

        // só imprime se mudou o nome da rua
        if (currentStreet != lastStreet) {
            step++;
            if (step == 1) std::cout << " " << step << ". comece na " << currentStreet << "\n";
            else std::cout << " " << step << ". siga para " << currentStreet << "\n";
            lastStreet = currentStreet;
        }
    }
    std::cout << " " << ++step << ". CHEGADA AO DESTINO!\n";
    std::cout << "================================================\n";
}
// Adaptador: assinatura deve ser exatamente SearchFn
std::vector<std::pair<std::string, long long>>
trieSearch(const std::string& query, int maxResults) {
    std::vector<std::pair<std::string, long long>> out;
    if (query.empty() || !g_trieRoot) return out;

    // Reutiliza o array estático que você já tem
    trie::SearchResult results[200];
    int count = 0;
    trie::search(g_trieRoot, query, results, &count);

    int limit = std::min(count, maxResults);
    for (int i = 0; i < limit; i++) {
        // nodeIds[0] é o primeiro ID associado à label
        out.push_back({ results[i].label, results[i].nodeIds[0] });
    }
    return out;
}
// Lê uma tecla s/n do terminal (sem precisar de Enter)
static char readConfirm() {
#ifdef _WIN32
    return (char)_getch();
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1; newt.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char c = 0;
    read(STDIN_FILENO, &c, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
#endif
}

long long selectLocation(const std::string& type, long long excludeId = -1, bool isDestino = false) {
    AutocompleteConfig cfg;
    cfg.prompt     = ">> " + type + ": ";
    cfg.maxVisible = 8;

    while (true) {
        auto res = autocomplete(cfg.prompt, trieSearch, cfg);

        if (res.cancelled) {
            if (isDestino) {
                std::cout << "tem certeza que deseja cancelar? [s/n] ";
                std::cout.flush();
                char c = readConfirm();
                std::cout << "\n";
                if (c == 's' || c == 'S') return -1; // volta para origem no loop principal
                continue; // continua buscando destino
            } else {
                std::cout << "tem certeza que deseja sair? [s/n] ";
                std::cout.flush();
                char c = readConfirm();
                std::cout << "\n";
                if (c == 's' || c == 'S') return -2; // sinal de "sair do programa"
                continue; // continua buscando origem
            }
        }

        if (res.id == excludeId) {
            std::cout << "   [!] origem e destino nao podem ser iguais.\n";
            continue;
        }

        if (res.id == -1) {
            std::cout << "   [!] nenhum resultado. tente outro nome.\n";
            continue;
        }

        std::cout << cfg.colorHighlight << cfg.prompt << cfg.colorNormal
                  << res.label << "\n";
        std::cout.flush();
        return res.id;
    }
}

std::string formatDistance(double meters) {
    if (meters < 1000.0) {
        return std::to_string((int)meters) + " metros";
    }

    double km = meters / 1000.0;
    // f mod = % but for double
    int isKmInt = std::fmod(km, 1.0) == 0.0 ? 1 : 0;
    if (isKmInt) {
        return std::to_string((int)km) + " km";
    }

    km = std::round(km * 10) / 10; // isn't int number, Round to 1 decimal
    std::string kmString = std::to_string(km); // "5.400000"
    int dotIndex = kmString.find('.');
    int endIndex = dotIndex + 2;
    // kmString.substr(startIdx, charsAfterStartToTake) (so we take from 0 to dot+2)
    // - lets say dot is index 1: in the string "1.2"
    // - if we use dot+1 we would get "1.", so we do dot+2: "1.2"
    return kmString.substr(0, endIndex) + " km";
}

int main() {
    setlocale(LC_ALL, "C");
    std::cout << "=== NAVPELOTAS 2026 ===\n";

    // carrega o grafo
    std::cout << "[1/2] carregando grafo... ";
    Graph* g = nullptr;
    try {
        std::string nPath = "data/nodes_formatado.json";
        std::string ePath = "data/edges_formatado.json";

        if (!std::filesystem::exists(nPath)) {
            nPath = "../data/nodes_formatado.json";
            ePath = "../data/edges_formatado.json";
        }

        g = new Graph(nPath, ePath);
        std::cout << "ok (" << g->size() << " nos)\n";
    } catch (const std::exception& e) {
        std::cerr << "\n[erro fatal grafo] " << e.what() << "\n";
        return 1;
    }

    // carrega a trie e o mapa
    std::cout << "[2/2] indexando ruas... ";
    trie::TrieNode* root = trie::init();
    g_trieRoot = root;
    try {
        fileModule::processNodesToLabel(root, loadDataCallback);
    } catch (...) {
        std::cerr << "\n[erro fatal trie]\n";
        return 1;
    }

    // debug to see if is sparse or dense
    int V = g->size();
    int E = g->totalEdges();
    double density = double(E) / (V * (V - 1)); // https://en.wikipedia.org/wiki/Dense_graph
    std::cout << "[INFO] vertices=" << V << ", arestas=" << E << ", densidade=" << density << "\n";

    std::cout << "\nsistema pronto!\n";

    // loop principal
    while(true) {
        long long originId = selectLocation("ORIGEM", -1);
        if (originId == -2) break;

        long long destId = selectLocation("DESTINO", originId,true);

        if (destId == -1) continue;

        try {
            int u = g->getIndexFromId(originId);
            int v = g->getIndexFromId(destId);

            std::cout << "\n[calculando rota...]\n";

            Dijkstra dj(*g, u);
            dj.execute();

            double dist = dj.getDistance(v);

            if (dist > 1e15) {
                std::cout << ">>> sem caminho possivel.\n";
            } else {
                std::cout << ">>> distancia: " << formatDistance(dist)<< "\n";
                auto path = dj.getPath(v);
                printDetailedRoute(g, path, v);
            }

        } catch (const std::exception& e) {
            std::cout << "[erro processamento] " << e.what() << "\n";
        }
    }

    // limpeza
    if (root) trie::destroy(root);
    if (g) delete g;

    std::cout << "\nencerrando.\n";
    return 0;
}
