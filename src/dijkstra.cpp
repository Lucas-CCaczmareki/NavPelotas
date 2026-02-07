#include "Dijkstra.h"
#include <algorithm>
#include <iostream>

// Construtor
// Recebe o grafo por referência e inicializa as estruturas internas do algoritmo
Dijkstra::Dijkstra(const Graph& g, int origin)   
    // Initializer list (obrigatória para referências)
    // Ordem deve seguir declaração no .h
    : graph(g),
    
      // Metadados
      numNodes(graph.size()),
      origin(origin),
      hasRun(false),

      // Estruturas auxiliares
      dist(numNodes, 1e18),         // menor distância conhecida da origem até cada nodo (infinita no início)
      prev(numNodes, {-1, -1}),     // vetor de reconstrução do caminho (prev[nodo atual] = {nodo anterior, aresta usada pra chegar no atual})
      explored(numNodes, false)     // guarda o estado dos nodos durante execução
{
    // Distância da origem até a origem é 0.
    dist[origin] = 0;            
}

/* Rascunho: Teoria Dijkstra 

    // 2-step process:
        // ---------------------------------------------------------------------------------------------------------
        // 1 - Update estimates
        // Vai conferir todas as arestas saindo do nodo atual e se o caminho até o próximo nodo
        // (considerando a soma do caminho pra chegar até o nodo atual, o custo acumulatório no caso)
        // for menor que o caminho registrado no dist[cur], atualiza ele com esse valor.
        // ---------------------------------------------------------------------------------------------------------

        // ---------------------------------------------------------------------------------------------------------
        // 2 - choose next vertex
        // Aqui a gente vai pra próxima cidade INEXPLORADA com o MENOR caminho pra lá 
        // (é aqui que a priority queue com heap entra pra eficiência, a gente evita de conferir todo o vetor de vizinhos)
        
        // ai quando a gente escolhe esse vértice e nos movemos pra cá, atualiza o custo acumulatório de caminho com 
        // o valor da aresta que usamos pra chegar aqui, e ai loopa o passo 1, de atualizar estimativas
        // ---------------------------------------------------------------------------------------------------------

*/

// Executa o Dijkstra a partir do valor da origem atual
// Calcula o menor caminho da origem para todos outros nodos alcançáveis
void Dijkstra::execute() {
    std::cout << "executing dijkstra...\n";
    
    // Recria a priority queue pra zerar ela entre execuções
    pq = std::priority_queue<pqNode>();
    pq.push({origin, 0});
    dist[origin] = 0;

    while(!pq.empty()) {

        // Seleciona o próximo nodo não explorado com menor distância
        pqNode cur_node = pq.top();
        pq.pop();

        // Descarta os caminhos (soma até aqui) piores que o melhor caminho já conhecido
        if (cur_node.path_weight > dist[cur_node.id]) continue;

        // Descarta entradas antigas da priority queue
        if (explored[cur_node.id]) continue;

        // Marca o nodo atual como explorado (finalizado)
        explored[cur_node.id] = true;

        int i = 0;
        // Atualização das estimativas
        for(auto& edge : graph.neighbours(cur_node.id)) {
            // se o peso desse caminho é menor que a menor distância conhecida, atualiza
            // OBS: Se houver mais de um caminho ótimo e eu quiser garantir que ele escolha sempre o ultimo caminho
            // é só trocar a comparação pra <=. Qual ele escolhe vai depender disso e outros fatores, mas é arbitrário, sempre vai vir o mesmo
            if((dist[cur_node.id] + edge.weight) < dist[edge.to]) {
                
                // Dist mantém a soma do menor caminho da origem ate o idx dela atualizado
                dist[edge.to] = dist[cur_node.id] + edge.weight;
                
                // atualiza o prev
                prev[edge.to].node = cur_node.id;
                prev[edge.to].edgeIdx = i;

                // Coloca os vizinhos na priority queue
                pq.push({edge.to, (dist[cur_node.id] + edge.weight)});
            }
            i++; //atualiza o indice da edge
        }
    }

    hasRun = true;
    std::cout << "dijkstra executed.\n";
}

// Retorna o valor da menor distância da origem até o destino
// Condição: O execute() já precisa ter sido chamado. (hasRun == true)
double Dijkstra::getDistance(int destination) const {
    return dist[destination];
}

// Reconstrói o menor caminho da origem até o desitno
// Retorna um vetor vazio se o destino for inalcançável
std::vector<Dijkstra::Prev> Dijkstra::getPath(int destination) const {
    // Proteção: destino inalcançável
    if (dist[destination] == 1e18) return {};

    // Prev {id_nodeAnterior, arestaUsada}
    // Monta o vetor de retorno
    std::vector<Dijkstra::Prev> path;
    int cur = destination;

    // Loop inicia do prev[destination], adiciona nodos e move 'para trás' até prev = origem
    while(cur != origin) {
        path.push_back(prev[cur]);     // coloca o nodo no caminho
        cur = prev[cur].node;          // vai um nodo pra trás no caminho
    }

    // Inverte o vetor para o sentido origem -> destino
    // O reverse inverte num intervalo [inicio, fim)
    std::reverse(path.begin(), path.end());

    // vou fazer ele printar o path só pra testes aqui por enquanto q vai ser mais fácil
    for(auto& path_node : path) {
        std::cout << graph.getIdFromIndex(path_node.node) << ", ";
    }
    std::cout << graph.getIdFromIndex(destination) << "\n";

    return path;
}

// Atualiza a origem do caminho pro algoritmo
// Invalida o resultado da última execução
void Dijkstra::setOrigin(int origin) {
    this->origin = origin;
    this->hasRun = false;
}