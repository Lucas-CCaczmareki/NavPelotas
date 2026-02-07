// Include guard
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

// Bibliotecas
#include <vector>
#include <queue>
#include <utility>
#include <functional>
#include "Graph.h"

// Implementa o algoritmo de Dijkstra sobre um grafo ponderado e direcionado
class Dijkstra {

public:
    // Elemento da priority queue
    // Ordenação pelo menor peso de caminho (min-heap) através da definição de comportamento do operador '<'
    struct pqNode {
        int id;
        double path_weight;

        // Define o comportamento do operador '<' ao comparar pqNodes
        bool operator<(const pqNode& other) const {
            // if: node é < que esse outro.node? 
            // return: ele é menor (menos prioridade) se o peso (path_weight) dele for maior
            return path_weight > other.path_weight;
        }
    };

    // Estrura para montar o vetor que reconstrói o menor caminho
    struct Prev {
        int node;       // nodo anterior no caminho
        int edgeIdx;    // índice da aresta usada (lista de adjacência adj[x][edgeIdx])
    };

    // Cria uma instância do Dijkstra associada a um grafo
    // Inicializa os atributos internos com valores padrão.
    Dijkstra(const Graph& g, int origin);

    // Executa o algoritmo de Dijkstra a partir do nodo de origem
    // Calcula a menor distância da origem para todos outros nodes
    // Utiliza fila de prioridade (minheap) para eficiência
    // Após execução: vetores dist e prev estão preenchidos
    void execute();

    // Retorna o valor da menor distância da origem até o destino
    // Condição: O execute() já precisa ter sido chamado. (hasRun == true)
    double getDistance(int destination) const;

    // Reconstrói o caminho mínimo da origem até o destino
    // Condição: O execute() já precisa ter sido chamado. (hasRun == true)
    // Retorna vetor vazio se o destino for inalcançável
    std::vector<Prev> getPath(int destination) const;

    // Atualiza o nodo de origem e invalida o resultado anterior
    void setOrigin(int origin);

private:
    // Referência para o grafo associado. (evita passar ele pro Dijkstra toda hora)
    const Graph& graph;

    // Metadados
    int numNodes;
    int origin;
    bool hasRun;    

    // Vetor para guardar as menores distâncias conhecidas
    // dist[i]  = menor distância conhecida da origem até o nodo i
    std::vector<double> dist;

    // Vetor com as informações para reconstruir o menor caminho
    // prev[nodo atual] = {nodo anterior, aresta percorrida}
    std::vector<Prev> prev;

    // Marca os nodos já explorados pelo algoritmo durante execução
    std::vector<bool> explored;

    // Fila de prioridade utilizada pelo Dijkstra
    /* Comportamento: Min-Heap
        // A priority_queue tem 3 parâmetros: tipo de elemento, container, comparação entre os elementos
        // Aqui a gente especifica só o tipo do elemento(pqNode) e a pq vai usar a comparação padrão que é less '<'
        // Como nossa struct pqNode tem uma definição de comportamento pro operator <, ela vai funcionar como min-heap
    */
    std::priority_queue<pqNode> pq;
};

#endif