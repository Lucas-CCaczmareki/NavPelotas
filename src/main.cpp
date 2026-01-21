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
}
