#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include <queue>
#include <utility>
#include <functional>
#include "Graph.h"

class Dijkstra {

public:
    struct pqNode {
        int id;
        double path_weight;

        // Isso aqui vai definir o comportamento do operador < quando eu comparar 2 pqNode
        bool operator<(const pqNode& other) const {
            // if: node é < que esse outro.node? 
            // return: ele é menor (menos prioridade) se o peso (path_weight) dele for maior
            return path_weight > other.path_weight;
        }
    };

    struct Prev {
        int node;
        int edgeIdx; //index na lista de adjacência da aresta pela qual veio .
    };

    // Construtor que recebe um grafo
    Dijkstra(const Graph& g, int origin);

    // Função pra executar o algoritmo
    void execute();

    // Getter para o valor do menor caminho entre A e B
    double getDistance(int destination);

    // Getter para reconstruir o menor caminho entre A e B
    std::vector<Prev> getPath(int destination);

    // Setter pra mudar a origin
    void setOrigin(int origin);

private:
    // Guardando o grafo como referência (pra n passar ela toda hora)
    const Graph& graph;

    // Metadados
    int numNodes;
    int origin;
    bool hasRun;    // indica se o algoritmo já rodou para a última origin inserida

    // Vetor para guardar as menores distâncias conhecidas 
    std::vector<double> dist;

    // Vetor para reconstruir o menor caminho
    std::vector<Prev> prev;

    // Vetor pra conferir se as cidades já foram exploradas
    std::vector<bool> explored;

    // Priority queue usando heap para escolher o próximo nodo sem percorrer todos para escolher o melhor
    // Definindo o comporatamento da nossa priority queue (a padrão é max-heap, e o dijkstra precisa da minheap)
    // -----------------------------------------------------------------------------------------------------------
    // A priority_queue tem 3 parâmetros: tipo de elemento, container, comparação entre os elementos
    // Aqui a gente especifica só o tipo do elemento(pqNode) e a pq vai usar a comparação padrão que é less '<'
    // Como nossa struct pqNode tem uma definição de comportamento pro operator <, ela vai funcionar como min-heap
    // -----------------------------------------------------------------------------------------------------------
    std::priority_queue<pqNode> pq;
};

#endif