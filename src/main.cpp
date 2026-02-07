/*
// OBS: O main foi feito usando IA pra testar os caminhos por enquanto e conseguir ligar com
// os múltiplos ids que podem vir da trie

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include "Graph.h"
#include "json.hpp"
#include <unordered_map>
#include "Dijkstra.h"

int main() {
    try {
        Graph g("data/nodes.json", "data/edges.json");
        
        // Origem com múltiplos IDs (exemplo: joao gomes nogueira x rotula)
        std::vector<long long> origem_ids = {1675174988};
        
        // Destino com múltiplos IDs (bento goncalves x goncalves chaves)
        std::vector<long long> destino_ids = {1675174980};
        
        // Variáveis para armazenar o melhor resultado global
        double menor_distancia = std::numeric_limits<double>::max();
        int melhor_origem_idx = -1;
        int melhor_destino_idx = -1;
        long long melhor_origem_id = -1;
        long long melhor_destino_id = -1;
        Dijkstra* melhor_dj = nullptr;
        
        std::cout << "Testando todas combinações de origem/destino:\n";
        std::cout << "==============================================\n\n";
        
        // Testa todas combinações de origem x destino
        for (size_t i = 0; i < origem_ids.size(); i++) {
            try {
                int o = g.getIndexFromId(origem_ids[i]);
                
                // Cria e executa Dijkstra para essa origem
                Dijkstra* dj = new Dijkstra(g, o);
                dj->execute();
                
                std::cout << "Origem ID " << origem_ids[i] << ":\n";
                
                // Testa cada destino
                for (size_t j = 0; j < destino_ids.size(); j++) {
                    try {
                        int d = g.getIndexFromId(destino_ids[j]);
                        double dist = dj->getDistance(d);
                        
                        std::cout << "  -> Destino ID " << destino_ids[j] 
                                  << ": " << dist << "m\n";
                        
                        // Atualiza se for o menor caminho
                        if (dist < menor_distancia) {
                            // Deleta o Dijkstra anterior se existir
                            if (melhor_dj != nullptr && melhor_dj != dj) {
                                delete melhor_dj;
                            }
                            
                            menor_distancia = dist;
                            melhor_origem_idx = o;
                            melhor_destino_idx = d;
                            melhor_origem_id = origem_ids[i];
                            melhor_destino_id = destino_ids[j];
                            melhor_dj = dj;
                        }
                    } catch(const std::exception& e) {
                        std::cout << "  -> Destino ID " << destino_ids[j] 
                                  << ": ERRO\n";
                    }
                }
                
                // Se esse Dijkstra não é o melhor, deleta
                if (dj != melhor_dj) {
                    delete dj;
                }
                
                std::cout << "\n";
                
            } catch(const std::exception& e) {
                std::cout << "Origem ID " << origem_ids[i] << ": ERRO - " 
                          << e.what() << "\n\n";
            }
        }
        
        // Imprime o melhor resultado
        std::cout << "==============================================\n";
        std::cout << "MELHOR CAMINHO ENCONTRADO:\n";
        std::cout << "==============================================\n";
        std::cout << "Origem ID: " << melhor_origem_id << "\n";
        std::cout << "Destino ID: " << melhor_destino_id << "\n";
        std::cout << "Distancia: " << menor_distancia << "m\n\n";
        std::cout << "Caminho:\n";
        
        if (melhor_dj != nullptr) {
            melhor_dj->getPath(melhor_destino_idx);
            delete melhor_dj;
        }
        
    } catch(const std::exception& e) {
        std::cerr << "Erro: " << e.what() << '\n';
    }
    
    return 0;
}*/
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
#include "Graph.h"
#include "trie.h"
#include "fileModule.h"
#include "Dijkstra.h"

// mapa para busca reversa (id -> nome da rua)
std::unordered_map<unsigned long long, std::string> idToLabelMap;

// callback para popular a trie e o mapa reverso simultaneamente
// chamada pelo filemodule
void loadDataCallback(trie::TrieNode* root, std::string label, unsigned long long id) {
    trie::insertWord(root, label, id);
    idToLabelMap[id] = label;
}

// leitura segura para evitar sujeira no buffer do teclado
std::string safeReadLine(const std::string& msg) {
    std::string input;
    while (true) {
        std::cout << msg << std::flush;
        if (std::cin.fail()) std::cin.clear();

        if (!std::getline(std::cin, input)) {
            std::cout << "\n[!] fim de entrada (eof). encerrando.\n";
            exit(0);
        }
        if (!input.empty() && input.back() == '\r') input.pop_back();
        if (!input.empty()) return input;
    }
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

// interface de busca e seleção via trie
long long selectLocation(trie::TrieNode* root, const std::string& type) {
    trie::SearchResult results[100];
    int count = 0;

    while (true) {
        std::string query = safeReadLine("\n>> digite a " + type + " (ou 'sair'): ");
        if (query == "sair") return -1;

        trie::search(root, query, results, &count);

        if (count == 0) {
            std::cout << "   [!] nada encontrado. tente outro nome.\n";
            continue;
        }

        std::cout << "   encontramos " << count << " opcoes:\n";
        int limit = (count > 10) ? 10 : count;

        for (int i = 0; i < limit; i++) {
            std::cout << "   [" << i << "] " << results[i].label << "\n";
        }

        // loop de selecao
        while (true) {
            std::string choiceStr = safeReadLine("   >> digite o numero (0-" + std::to_string(limit - 1) + ") ou 'c' para cancelar: ");

            // volta para a pesquisa se o usuario desistir
            if (choiceStr == "c" || choiceStr == "cancelar") {
                std::cout << "   [!] selecao cancelada.\n";
                break;
            }

            try {
                int choice = std::stoi(choiceStr);
                if (choice >= 0 && choice < count) {
                    long long id = results[choice].nodeIds[0];
                    std::cout << "   -> selecionado: " << results[choice].label << "\n";
                    return id;
                }
                std::cout << "   [!] numero invalido.\n";
            } catch (...) { std::cout << "   [!] digite um numero ou 'c'.\n"; }
        }
    }
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
    try {
        fileModule::processNodesToLabel(root, loadDataCallback);
    } catch (...) {
        std::cerr << "\n[erro fatal trie]\n";
        return 1;
    }

    std::cout << "\nsistema pronto!\n";

    // loop principal
    while(true) {
        long long originId = selectLocation(root, "ORIGEM");
        if (originId == -1) break;

        long long destId = selectLocation(root, "DESTINO");
        if (destId == -1) break;

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
                std::cout << ">>> distancia: " << dist << " metros\n";
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
