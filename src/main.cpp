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
#include <limits>
#include <filesystem>
#include <unordered_map> // <--- NOVO: Para o dicionário reverso
#include "Graph.h"
#include "trie.h"
#include "fileModule.h"
#include "Dijkstra.h"

// --- NOVO: Dicionário Reverso (ID -> Nome da Rua) ---
// Usamos isso para descobrir o nome da rua quando o Dijkstra devolve um ID.
std::unordered_map<unsigned long long, std::string> mapaNomesRuas;

// Callback modificado: Insere na Trie E no Mapa reverso ao mesmo tempo
void processarDadosRua(trie::TrieNode* root, std::string label, unsigned long long id) {
    // 1. Insere na Trie (para busca)
    trie::insertWord(root, label, id);

    // 2. Insere no Mapa (para exibir rota depois)
    mapaNomesRuas[id] = label;
}

// --- FUNÇÃO DE LEITURA BLINDADA ---
std::string lerLinhaSegura(const std::string& mensagem) {
    std::string entrada;
    while (true) {
        std::cout << mensagem << std::flush;
        if (std::cin.fail()) std::cin.clear();

        if (!std::getline(std::cin, entrada)) {
            std::cout << "\n[!] Fim da entrada de dados (EOF). Encerrando.\n";
            exit(0);
        }

        if (!entrada.empty() && entrada.back() == '\r') entrada.pop_back();

        if (!entrada.empty()) return entrada;
    }
}

// --- NOVO: Função para imprimir a rota bonitinha ---
void imprimirRotaDetalhada(Graph* g, const std::vector<Dijkstra::Prev>& path, int idxDestino) {
    std::cout << "\n==============================================\n";
    std::cout << "               ROTA DETALHADA                 \n";
    std::cout << "==============================================\n";

    std::string ultimaRua = "";
    double distanciaAcumulada = 0;
    int passos = 0;

    // Reconstrói o caminho de trás pra frente (o Dijkstra guarda o "pai" de cada nó)
    // Precisamos inverter ou seguir do destino até a origem.
    // O seu Dijkstra já tem getPath que devolve ordenado?
    // Assumindo que seu getPath retorna vetor de índices ordenados da Origem -> Destino:
    // Se o seu Dijkstra retorna Prev struct, precisamos reconstruir.
    // Vou usar a lógica padrão de reconstrução aqui para garantir:

    std::vector<int> caminhoOrdenado;
    int atual = idxDestino;

    // Se o seu Dijkstra.h não expõe o vetor 'previous', use o método getPath se ele retornar vector<int>
    // Vou assumir que você implementou um getPath que retorna vector<int> ou algo similar.
    // Se não, vamos usar a lógica simples:
    // (Adapte esta parte se o seu getPath for diferente)

    // ATENÇÃO: Estou assumindo que dj.getPath retorna um vector<int> ou similar.
    // Vou usar uma lógica genérica de impressão baseada no vetor que você já usava no código anterior.

    // No código anterior você iterava sobre 'path'. Vamos usar isso.

    for (size_t i = 0; i < path.size(); i++) {
        int u_idx = path[i].node; // Ajuste conforme sua struct Prev

        long long osmId = g->getIdFromIndex(u_idx);

        // Tenta achar o nome da rua
        std::string nomeAtual = "Rua sem nome (ID " + std::to_string(osmId) + ")";
        if (mapaNomesRuas.count(osmId)) {
            nomeAtual = mapaNomesRuas[osmId];
        }

        // Só imprime se mudou de rua (para não imprimir "Rua Bento" 50 vezes seguidas)
        if (nomeAtual != ultimaRua) {
            passos++;
            if (i == 0) {
                std::cout << " " << passos << ". Comece na " << nomeAtual << "\n";
            } else {
                std::cout << " " << passos << ". Siga para " << nomeAtual << "\n";
            }
            ultimaRua = nomeAtual;
        }
    }

    std::cout << " " << ++passos << ". CHEGADA AO DESTINO!\n";
    std::cout << "==============================================\n";
}

// ... Função selecionarLocal IGUAL AO ANTERIOR ...
long long selecionarLocal(trie::TrieNode* root, const std::string& tipo) {
    trie::SearchResult resultados[100];
    int qtdResultados = 0;

    while (true) {
        std::string busca = lerLinhaSegura("\n>> Digite o nome da " + tipo + " (ou 'sair'): ");
        if (busca == "sair") return -1;

        trie::search(root, busca, resultados, &qtdResultados);

        if (qtdResultados == 0) {
            std::cout << "   [!] Nada encontrado. Tente partes do nome.\n";
            continue;
        }

        std::cout << "   Encontramos " << qtdResultados << " opcoes:\n";
        int limite = (qtdResultados > 10) ? 10 : qtdResultados;

        for (int i = 0; i < limite; i++) {
            std::cout << "   [" << i << "] " << resultados[i].label << "\n";
        }

        while (true) {
            std::string escolhaStr = lerLinhaSegura("   >> Digite o NUMERO (0-" + std::to_string(limite-1) + "): ");
            try {
                int escolha = std::stoi(escolhaStr);
                if (escolha >= 0 && escolha < qtdResultados) {
                    long long id = resultados[escolha].nodeIds[0];
                    std::cout << "   -> OK: " << resultados[escolha].label << "\n";
                    return id;
                }
                std::cout << "   [!] Numero invalido.\n";
            } catch (...) { std::cout << "   [!] Digite apenas numeros.\n"; }
        }
    }
}

int main() {
    setlocale(LC_ALL, "C");
    std::cout << "=== NAVPELOTAS - ROTA DETALHADA ===\n";

    // 1. Grafo
    std::cout << "[1/2] Carregando Grafo... ";
    Graph* g = nullptr;
    try {
        // Ajuste os caminhos se precisar
        std::string n = "../data/nodes_formatado.json";
        std::string e = "../data/edges_formatado.json";

        // Verifica se existe na pasta local primeiro
        if (std::filesystem::exists("data/nodes_formatado.json")) {
            n = "data/nodes_formatado.json";
            e = "data/edges_formatado.json";
        }

        g = new Graph(n, e);
        std::cout << "OK (" << g->size() << " nos)\n";
    } catch (const std::exception& e) {
        std::cerr << "\nERRO GRAFO: " << e.what() << "\n";
        return 1;
    }

    // 2. Trie + Mapa
    std::cout << "[2/2] Carregando Ruas... ";
    trie::TrieNode* root = trie::init();
    try {
        // Passamos nossa função nova que preenche o MAPA também
        fileModule::processNodesToLabel(root, processarDadosRua);
    } catch (...) {
        std::cerr << "\nERRO TRIE.\n"; return 1;
    }

    std::cout << "\nSistema Pronto!\n";

    while(true) {
        long long idOrigem = selecionarLocal(root, "ORIGEM");
        if (idOrigem == -1) break;

        long long idDestino = selecionarLocal(root, "DESTINO");
        if (idDestino == -1) break;

        try {
            int idxOrigem = g->getIndexFromId(idOrigem);
            int idxDestino = g->getIndexFromId(idDestino);

            std::cout << "\n[Calculando rota...]\n";

            Dijkstra dj(*g, idxOrigem);
            dj.execute();

            double dist = dj.getDistance(idxDestino);

            if (dist > 1e15) {
                std::cout << ">>> Sem caminho possivel.\n";
            } else {
                std::cout << ">>> Distancia: " << dist << " metros\n";

                // Pega o caminho (Adapte se seu Dijkstra retornar vector<int>)
                // Aqui assumo que você tem um método getPath que retorna vector<Prev> ou vector<int>
                // Se seu getPath retorna vector<int> (indices), use assim:

                // Exemplo se getPath retorna indices:
                /* std::vector<int> caminho = dj.getPath(idxDestino);
                std::vector<Dijkstra::Prev> caminhoAdaptado;
                for(int idx : caminho) caminhoAdaptado.push_back({idx, 0});
                imprimirRotaDetalhada(g, caminhoAdaptado, idxDestino);
                */

                // Se o seu getPath retorna o struct Prev (como no codigo anterior):
                auto caminho = dj.getPath(idxDestino);
                imprimirRotaDetalhada(g, caminho, idxDestino);
            }

        } catch (const std::exception& e) {
            std::cout << "Erro: " << e.what() << "\n";
        }
    }

    return 0;
}
